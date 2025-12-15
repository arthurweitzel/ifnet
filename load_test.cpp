#include <arpa/inet.h>
#include <atomic>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>

using namespace std;

struct TestConfig {
    int num_clients = 100;
    int messages_per_client = 10;
    int num_threads = 10;
    string host = "127.0.0.1";
    int port = 1337;
    int delay_ms = 0;
};

struct TestMetrics {
    // atomic lida com as operacoes de forma threadsafe
    atomic<int> successful_connections{0};
    atomic<int> failed_connections{0};
    atomic<int> messages_sent{0};
    atomic<int> messages_failed{0};
    atomic<long long> total_connection_time_ms{0};

    void print_report(double elapsed_seconds) const {
        int total_conn = successful_connections + failed_connections;
        int total_msg = messages_sent + messages_failed;
        
        double conn_rate = total_conn > 0 ? (100.0 * successful_connections / total_conn) : 0.0;
        double msg_rate = total_msg > 0 ? (100.0 * messages_sent / total_msg) : 0.0;
        double throughput = elapsed_seconds > 0 ? messages_sent / elapsed_seconds : 0.0;
        double avg_lat = successful_connections > 0 ? (double)total_connection_time_ms / successful_connections : 0.0;
        double conn_sec = elapsed_seconds > 0 ? successful_connections / elapsed_seconds : 0.0;

        cout << "TESTE ===\n";
        cout << "Conexoes: " << successful_connections << " ok, " << failed_connections << " falhas (" << conn_rate << "%)\n";
        cout << "Mensagens: " << messages_sent << " ok, " << messages_failed << " falhas (" << msg_rate << "%)\n";
        cout << "Tempo: " << elapsed_seconds << "s\n";
        cout << "Throughput: " << throughput << " msg/s\n";
        cout << "Latencia media: " << avg_lat << " ms\n";
        cout << "Conexoes/seg: " << conn_sec << "\n";
    }
};

struct SocketGuard {
    int fd;
    SocketGuard() : fd(socket(AF_INET, SOCK_STREAM, 0)) {}
    ~SocketGuard() { if (fd >= 0) close(fd); }
    bool is_valid() const { return fd >= 0; }
};

void simulate_client(int client_id, const TestConfig &config, TestMetrics &metrics) {
    SocketGuard sock;
    if (!sock.is_valid()) {
        metrics.failed_connections++;
        return;
    }

    sockaddr_in server_addr{}; 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config.port);

    if (inet_pton(AF_INET, config.host.c_str(), &server_addr.sin_addr) <= 0) {
        metrics.failed_connections++;
        return; 
    }

    // pra medir o tempo de conexao 
    auto start = chrono::high_resolution_clock::now();
    
    if (connect(sock.fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        metrics.failed_connections++;
        return;
    }

    // fim da conexao
    auto end = chrono::high_resolution_clock::now();
    metrics.successful_connections++;
    metrics.total_connection_time_ms += chrono::duration_cast<chrono::microseconds>(end - start).count();

    for (int i = 0; i < config.messages_per_client; ++i) {
        string msg = "Client " + to_string(client_id) + " Msg " + to_string(i + 1);
        
        // antes eu tinha usado write(), mas send é mais moderno/seguro
        ssize_t sent = send(sock.fd, msg.c_str(), msg.size(), 0);

        if (sent > 0) metrics.messages_sent++;
        else metrics.messages_failed++;

        if (config.delay_ms > 0) {
            this_thread::sleep_for(chrono::milliseconds(config.delay_ms));
        }
    }
}

void worker_thread(int start_id, int end_id, const TestConfig &config, TestMetrics &metrics) {
    for (int i = start_id; i < end_id; ++i) {
        simulate_client(i, config, metrics);
    }
}


// argc = quantidade de argumentos, argv = vetor com os argumentos
TestConfig parse_args(int argc, char *argv[]) {
    TestConfig c;
    
    // comeca em 1 pq argv[0] é o nome do programa
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        
        // verifica se tem proximo argumento ja que --clients precisa de um valor
        if (i + 1 < argc) {
            if (arg == "--clients")       c.num_clients = stoi(argv[++i]);
            else if (arg == "--messages") c.messages_per_client = stoi(argv[++i]);
            else if (arg == "--threads")  c.num_threads = stoi(argv[++i]);
            else if (arg == "--host")     c.host = argv[++i];
            else if (arg == "--port")     c.port = stoi(argv[++i]);
            else if (arg == "--delay")    c.delay_ms = stoi(argv[++i]);
        }
    }
    return c;
}

int main(int argc, char *argv[]) {
    TestConfig config = parse_args(argc, argv);
    TestMetrics metrics;

    cout << "Iniciando teste: " << config.num_clients << " clientes, " 
         << config.num_threads << " threads\n";

    vector<thread> threads;
    
    auto start_time = chrono::high_resolution_clock::now();
    int clients_per_thread = config.num_clients / config.num_threads;
    int remaining = config.num_clients % config.num_threads;  
    int current_client = 0;

    for (int t = 0; t < config.num_threads; ++t) {
        int count = clients_per_thread + (t < remaining ? 1 : 0);
        
        // emplace_back cria a thread direto no vetor
        // std::ref passa as referencias pro worker (senao faz copia)
        threads.emplace_back(worker_thread, current_client, current_client + count, 
                             ref(config), ref(metrics));
        current_client += count;
    }

    for (auto &t : threads) {
        t.join();
    }

    auto end_time = chrono::high_resolution_clock::now();
    double elapsed = chrono::duration<double>(end_time - start_time).count();

    metrics.print_report(elapsed);

    return 0;
}

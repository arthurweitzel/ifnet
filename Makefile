# Compilador
CXX = g++

# Flags de compilação:
# -Iinclude      -> Adiciona o diretório 'include' à busca por headers (#include "...")
# -std=c++17     -> Usa o padrão C++17
# -Wall -Wextra  -> Ativa todos os warnings úteis
# -g             -> Inclui símbolos de debug para usar com GDB
CXXFLAGS = -std=c++17 -Wall -Wextra -g -Iinclude

# Flags de linkagem (para a thread library)
LDFLAGS = -pthread

# Nome do executável final
EXECUTABLE = server

# Diretórios
SRC_DIR = src
OBJ_DIR = obj

# Encontra automaticamente todos os arquivos .cpp no diretório src
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Gera os nomes dos arquivos objeto (.o) a partir dos fontes, colocando-os no diretório obj
# Ex: src/main.cpp se torna obj/main.o
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# --- REGRAS ---

# A regra padrão (o que `make` faz sem argumentos)
all: $(EXECUTABLE)

# Regra para linkar o executável final a partir dos arquivos objeto
$(EXECUTABLE): $(OBJECTS)
	@echo "LD   $@"
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build finalizado. Execute com: ./$(EXECUTABLE)"

# Regra de padrão para compilar arquivos .cpp de 'src' para arquivos .o em 'obj'
# A dependência '| $(OBJ_DIR)' garante que o diretório obj/ seja criado antes da compilação
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "CXX  $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para criar o diretório de objetos
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Regra para limpar os arquivos gerados
clean:
	@echo "Cleaning project..."
	@rm -rf $(OBJ_DIR) $(EXECUTABLE)

# Declara que 'all' e 'clean' não são nomes de arquivos
.PHONY: all clean

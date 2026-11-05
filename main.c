#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENCOMENDAS 100
#define NOME_ARQUIVO "encomendas.txt"

// Estrutura principal da encomenda
typedef struct {
    int id;
    char destinatario[50];
    char endereco[100];
    float peso;
    float valor_frete;
    char status[30];
} Encomenda;

// Vetor global de encomendas e contador
Encomenda lista[MAX_ENCOMENDAS];
int total = 0;

// ============================================================================
// FUNÇÕES AUXILIARES DE ENTRADA SEGURA
// ============================================================================

// Lê um inteiro com validação (impede loop infinito em caso de texto)
int ler_inteiro(const char *mensagem) {
    int valor;
    char buffer[100];
    while (1) {
        printf("%s", mensagem);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%d", &valor) == 1) {
                return valor;
            }
        }
        printf("[ERRO] Entrada invalida! Digite apenas numeros inteiros.\n");
    }
}

// Lê um inteiro garantindo que esteja dentro de um intervalo [min, max]
int ler_inteiro_faixa(const char *mensagem, int min, int max) {
    int valor;
    while (1) {
        valor = ler_inteiro(mensagem);
        if (valor >= min && valor <= max) {
            return valor;
        }
        printf("[ERRO] Opcao invalida! Digite um numero entre %d e %d.\n", min, max);
    }
}

// Lê um float estritamente positivo (> 0)
float ler_float_positivo(const char *mensagem) {
    float valor;
    char buffer[100];
    while (1) {
        printf("%s", mensagem);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%f", &valor) == 1 && valor > 0.0f) {
                return valor;
            }
        }
        printf("[ERRO] Entrada invalida! Digite um numero decimal maior que zero (ex: 1.5).\n");
    }
}

// Lê texto com segurança, remove '\n' e substitui ';' por ',' para proteger o .txt (CSV)
void ler_string_segura(const char *mensagem, char *destino, int tamanho) {
    char buffer[200];
    while (1) {
        printf("%s", mensagem);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = '\0'; // Remove quebra de linha
            
            // Remove espaços no início
            char *inicio = buffer;
            while (*inicio == ' ' || *inicio == '\t') inicio++;
            
            if (strlen(inicio) == 0) {
                printf("[ERRO] O campo nao pode ser vazio! Tente novamente.\n");
                continue;
            }

            // Sanitiza caractere ';' para não quebrar a estrutura do arquivo .txt
            for (int i = 0; inicio[i] != '\0'; i++) {
                if (inicio[i] == ';') {
                    inicio[i] = ','; // Substitui ponto e vírgula por vírgula
                }
            }

            strncpy(destino, inicio, tamanho - 1);
            destino[tamanho - 1] = '\0';
            return;
        }
    }
}

// ============================================================================
// FUNÇÕES DE NEGÓCIO DA LOGÍSTICA
// ============================================================================

// Função para calcular o valor do frete
float calcular_frete(float peso, int regiao) {
    float taxa_base = 0.0;
    
    switch (regiao) {
        case 1: taxa_base = 15.00; break; // Sudeste
        case 2: taxa_base = 20.00; break; // Sul
        case 3: taxa_base = 25.00; break; // Centro-Oeste
        case 4: taxa_base = 30.00; break; // Nordeste
        case 5: taxa_base = 35.00; break; // Norte
        default: taxa_base = 20.00; break;
    }
    
    if (peso > 2.0f) {
        taxa_base += (peso - 2.0f) * 5.00f; // Adicional por kg excedente
    }
    
    return taxa_base;
}

// 1. Cadastrar Encomenda com entradas 100% validadas
void cadastrar_encomenda() {
    if (total >= MAX_ENCOMENDAS) {
        printf("\n[ERRO] Limite maximo de %d encomendas atingido!\n", MAX_ENCOMENDAS);
        return;
    }

    Encomenda e;
    e.id = 1001 + total; // ID gerado automaticamente

    printf("\n--- CADASTRO DE NOVA ENCOMENDA (ID: %d) ---\n", e.id);
    
    ler_string_segura("Nome do Destinatario: ", e.destinatario, sizeof(e.destinatario));
    ler_string_segura("Endereco de Entrega: ", e.endereco, sizeof(e.endereco));
    e.peso = ler_float_positivo("Peso do pacote em kg (ex: 2.5): ");

    int regiao = ler_inteiro_faixa("Regiao de Destino (1-Sudeste, 2-Sul, 3-Centro-Oeste, 4-Nordeste, 5-Norte): ", 1, 5);

    e.valor_frete = calcular_frete(e.peso, regiao);
    strcpy(e.status, "Aguardando coleta");

    lista[total] = e;
    total++;

    printf("\n[SUCESSO] Encomenda cadastrada! Frete: R$ %.2f | Status: %s\n", e.valor_frete, e.status);
}

// 2. Rastrear Encomenda por ID
void rastrear_encomenda() {
    if (total == 0) {
        printf("\n[AVISO] Nenhuma encomenda cadastrada ate o momento.\n");
        return;
    }

    int id_busca = ler_inteiro("\nDigite o ID da encomenda para rastrear (ex: 1001): ");
    int encontrado = 0;

    for (int i = 0; i < total; i++) {
        if (lista[i].id == id_busca) {
            printf("\n=========================================\n");
            printf("         DETALHES DA ENCOMENDA %d         \n", lista[i].id);
            printf("=========================================\n");
            printf("Destinatario : %s\n", lista[i].destinatario);
            printf("Endereco     : %s\n", lista[i].endereco);
            printf("Peso         : %.2f kg\n", lista[i].peso);
            printf("Valor Frete  : R$ %.2f\n", lista[i].valor_frete);
            printf("Status Atual : %s\n", lista[i].status);
            printf("=========================================\n");
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("\n[ERRO] Encomenda com ID %d nao foi encontrada.\n", id_busca);
    }
}
void listar_todas() { 
    if (total == 0) { 
        printf("\n[AVISO] Nenhuma encomenda cadastrada até o momento.\n"); 
        return; 
    } 
    // CORRIGIDO: Adicionado ', total' no final para exibir a quantidade real 
    printf("\n--- RELATORIA DE TODAS AS ENCOMENDAS (%d) ---\n", total); 
    for (int i = 0; i < total; i++) { 
        // CORRIGIDO: %.2f ajustado para o peso 
        printf("ID: %d | Destinatario: %s | Peso: %.2f kg | Frete: R$ %.2f | Status: %s\n",
             lista[i].id, lista[i].destinatario, lista[i].peso, lista[i].valor_frete, lista[i].status); 
            } 
        }

// 3. Atualizar Status da Entrega
void atualizar_status() {
    if (total == 0) {
        printf("\n[AVISO] Nenhuma encomenda cadastrada.\n");
        return;
    }

    int id_busca = ler_inteiro("\nDigite o ID da encomenda para atualizar o status: ");
    int encontrado = 0;

    for (int i = 0; i < total; i++) {
        if (lista[i].id == id_busca) {
            encontrado = 1;
            printf("\nStatus Atual: %s\n", lista[i].status);
            printf("Escolha o novo status:\n");
            printf("1. Em Transito\n");
            printf("2. Saiu para Entrega\n");
            printf("3. Entregue\n");
            
            int opcao_status = ler_inteiro_faixa("Opcao (1-3): ", 1, 3);

            switch (opcao_status) {
                case 1: strcpy(lista[i].status, "Em Transito"); break;
                case 2: strcpy(lista[i].status, "Saiu para Entrega"); break;
                case 3: strcpy(lista[i].status, "Entregue"); break;
            }
            printf("\n[SUCESSO] Status atualizado para: %s\n", lista[i].status);
            break;
        }
    }

    if (!encontrado) {
        printf("\n[ERRO] Encomenda com ID %d nao encontrada.\n", id_busca);
    }
}

// 4. Salvar Dados em Arquivo de Texto (.txt)
void salvar_arquivo() {
    FILE *arq = fopen(NOME_ARQUIVO, "w");
    if (arq == NULL) {
        printf("\n[ERRO] Nao foi possivel abrir o arquivo para escrita.\n");
        return;
    }

    for (int i = 0; i < total; i++) {
        fprintf(arq, "%d;%s;%s;%.2f;%.2f;%s\n",
                lista[i].id,
                lista[i].destinatario,
                lista[i].endereco,
                lista[i].peso,
                lista[i].valor_frete,
                lista[i].status);
    }

    fclose(arq);
    printf("\n[SUCESSO] %d encomenda(s) salva(s) com sucesso em '%s'!\n", total, NOME_ARQUIVO);
}

// 5. Carregar Dados do Arquivo (.txt)
void carregar_arquivo() {
    FILE *arq = fopen(NOME_ARQUIVO, "r");
    if (arq == NULL) {
        return; // Arquivo ainda nao existe
    }

    total = 0;
    while (fscanf(arq, "%d;%49[^;];%99[^;];%f;%f;%29[^\n]\n",
                  &lista[total].id,
                  lista[total].destinatario,
                  lista[total].endereco,
                  &lista[total].peso,
                  &lista[total].valor_frete,
                  lista[total].status) == 6) {
        total++;
        if (total >= MAX_ENCOMENDAS) break;
    }

    fclose(arq);
    if (total > 0) {
        printf("[SISTEMA] %d encomenda(s) carregada(s) do arquivo '%s'.\n", total, NOME_ARQUIVO);
    }
}

int main() {
    int opcao;

    // Carrega registros salvos anteriormente
    carregar_arquivo();

    do {
        printf("\n=========================================\n");
        printf("    SISTEMA DE LOGISTICA \n");
        printf("=========================================\n");
        printf("1. Cadastrar Nova Encomenda\n");
        printf("2. Consultar / Rastrear Pacote\n");
        printf("3. Listas Todas as Entregas\n");
        printf("4. Atualizar Status da Entrega\n");
        printf("5. Salvar Dados no Arquivo (.txt)\n");
        printf("0. Sair do Sistema\n");
        printf("-----------------------------------------\n");

        opcao = ler_inteiro_faixa("Escolha uma opcao (0-5): ", 0, 5);

        switch (opcao) {
            case 1: cadastrar_encomenda(); break;
            case 2: rastrear_encomenda(); break;
            case 3: listar_todas(); break;
            case 4: atualizar_status(); break;
            case 5: salvar_arquivo(); break;
            case 0:
                salvar_arquivo(); // Salva automaticamente ao sair
                printf("\nDados salvos. Encerrando o sistema... Ate logo!\n");
                break;
        }
    } while (opcao != 0);

    return 0;
}

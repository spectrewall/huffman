/* Arquivo: AlgoritmoDeHuffman.c / Diciplina: Estrutura de Dados II
 * Aluno: Wallace Luiz Carvalho de Andrade
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*
 *  Definicao de caractere de 0 a 255
*/
typedef unsigned char caractere;

/*
 *  Estrutura da Arvore de Huffman armazenando frequencia,
 *  o caractere a ser guardado, e os filhos a direita e a esquerda.
*/
typedef struct noh
{
    int freq;
    caractere c;
    struct noh *esq;
    struct noh *dir;
} noh;

/*
 *  Estrutura da Lista armazenando um no da arvore de Huffman,
 *  e o proximo item da lista.
*/
typedef struct nohLista
{
    noh *n;
    struct nohLista *proximo;
} nohLista;

/*
 *  cabeca da lista contando ponteiro que aponta para o topo
 *  e a quantidade de elementos.
*/
typedef struct lista
{
    nohLista *topo;
    int elementos;
} lista;

/*
 *  Funcao que concatena duas strings e retorna a string resultante.
 *  utilizada na função "imprimeCodigo".
*/
char *str_concat(char *str1, const char *str2)
{
    char *buffer = (char *)malloc(8 * sizeof(char));
    strcpy(buffer, str1);
    strcat(buffer, str2);

    return buffer;
}

/*
 *  Funcao que percorre recursivamente a Arvore de Huffman imprimindo
 *  o caractere que foi comprimido, a frequencia em que ele ocorre, e o seu codigo
 *  de huffman.
*/
void imprimeCodigo(noh *arvore, char *codigo)
{
    int i;

    if (arvore != NULL)
    {
        imprimeCodigo(arvore->dir, str_concat(codigo, "1")); /* Percorre a direita acrescenta o valor 1 ao final do codigo */
        if (arvore->c != '#')
            printf("\n%c[%d]: %s", arvore->c, arvore->freq, codigo);
        imprimeCodigo(arvore->esq, str_concat(codigo, "0")); /* Percorre a esquerda acrescenta o valor 0 ao final do codigo */
    }
}

/*
 *  Aloca um novo noh para ser inserido na lista
*/
nohLista *novoNohLista(noh *no)
{
    nohLista *novo;
    if ((novo = malloc(sizeof(*novo))) == NULL)
        return NULL;

    novo->n = no;
    novo->proximo = NULL;

    return novo;
}

/*
 *  Aloca um novo noh para ser inserido na arvore
*/
noh *novoNoh(caractere c, int freq, noh *esq, noh *dir)
{
    noh *novo;

    if ((novo = malloc(sizeof(*novo))) == NULL)
        return NULL;

    novo->c = c;
    novo->freq = freq;
    novo->esq = esq;
    novo->dir = dir;

    return novo;
}

/*
 *  Insere um no na lista de prioridade
*/
void insereLista(nohLista *n, lista *l)
{
    if (!l->topo)
    {
        l->topo = n;
    }
    else if (n->n->freq < l->topo->n->freq)
    {
        n->proximo = l->topo;
        l->topo = n;
    }
    else
    {
        nohLista *aux = l->topo->proximo;
        nohLista *aux2 = l->topo;

        while (aux && aux->n->freq <= n->n->freq)
        {
            aux2 = aux;
            aux = aux2->proximo;
        }
        aux2->proximo = n;
        n->proximo = aux;
    }

    l->elementos++;
}

/*
 *  Remove o no de menor frequencia da lista,
 *  ou seja, o que esto no topo dela.
*/
noh *ExtraiMin(lista *l)
{
    nohLista *aux = l->topo;
    noh *aux2 = aux->n;
    l->topo = aux->proximo;
    free(aux);
    aux = NULL;
    l->elementos--;

    return aux2;
}

/*
 *  Calcula a frequencia de repeticoes dos caracteres no arquivo
*/
void calculaFrequencia(FILE *entrada, unsigned int *listaCaracteres)
{
    caractere c;

    while (fread(&c, 1, 1, entrada) >= 1)
    {
        listaCaracteres[(caractere)c]++;
    }
    rewind(entrada);
}

#include <stdbool.h>

/*
 *  Funcao que percorre a arvore recursivamente buscanco por
 *  um caractere e calculando o seu codigo de huffman
*/
bool pegaCodigo(noh *n, caractere c, char *buffer, int tamanho)
{
    if (!(n->esq || n->dir) && n->c == c)
    {
        buffer[tamanho] = '\0';
        return true;
    }
    else
    {
        bool encontrado = false;

        if (n->esq)
        {
            buffer[tamanho] = '0';
            encontrado = pegaCodigo(n->esq, c, buffer, tamanho + 1);
        }
        if (!encontrado && n->dir)
        {
            buffer[tamanho] = '1';
            encontrado = pegaCodigo(n->dir, c, buffer, tamanho + 1);
        }
        if (!encontrado)
        {
            buffer[tamanho] = '\0';
        }
        return encontrado;
    }
}

/*
 *  Funcao de huffman que pega os nos da lista de prioridade
 *  e vai inserindo na arvore.
*/
noh *CodigoDeHuffman(unsigned *listaCaracteres)
{
    lista l = {NULL, 0};

    for (int i = 0; i < 256; i++)
    {
        if (listaCaracteres[i])
        {
            insereLista(novoNohLista(novoNoh(i, listaCaracteres[i], NULL, NULL)), &l);
        }
    }

    while (l.elementos > 1)
    {
        noh *nodeEsquerdo = ExtraiMin(&l);
        noh *nodeDireito = ExtraiMin(&l);
        noh *soma = novoNoh('#', nodeEsquerdo->freq + nodeDireito->freq, nodeEsquerdo, nodeDireito);
        insereLista(novoNohLista(soma), &l);
    }

    return ExtraiMin(&l);
}

/*
 *  Funcao recursiva que percorre a arvore e
 *  libera da memoria todos os seus nos.
*/
void LiberaArvore(noh *n)
{
    if (!n)
        return;
    else
    {
        noh *esq = n->esq;
        noh *dir = n->dir;
        free(n);
        LiberaArvore(esq);
        LiberaArvore(dir);
    }
}

/*
 *  Funcao que verifica a existencia de um caractere na tabela
 *   de codigos armazenadas no arquivo hx para fazer sua descompressao
*/
int buscacaractere(FILE *entrada, int posicao, caractere *aux)
{
    (posicao % 8 == 0) ? fread(aux, 1, 1, entrada) : NULL == NULL;
    return !!((*aux) & (1 << (posicao % 8))); /* Retorna 1 caso seja diferente de 0, ou 0 caso seja igual a zero */
}

/*
 *  Funcao que le o arquivo de Entrada e faz a compressao desse arquivo
 *  em um novo arquivo com a terminacao .hx
*/
void ComprimeArquivo(const char *arquivoEntrada, const char *arquivoSaida)
{
    unsigned listaCaracteres[256] = {0};

    FILE *entrada = fopen(arquivoEntrada, "rb");
    if (entrada == NULL)
    {
        printf("Arquivo %s nao encontrado\n", arquivoEntrada);
    }
    else
    {
        FILE *saida = fopen(arquivoSaida, "wb");
        if (saida == NULL)
        {
            printf("\n\n*** ERRO AO CRIAR O ARQUIVO DE SAIDA! ***\n\n");
        }
        else
        {
            calculaFrequencia(entrada, listaCaracteres);
            noh *raiz = CodigoDeHuffman(listaCaracteres);
            fwrite(listaCaracteres, 256, sizeof(listaCaracteres[0]), saida);
            fseek(saida, sizeof(unsigned int), SEEK_CUR);
            caractere c;
            unsigned tamanho = 0;
            caractere aux = 0;

            while (fread(&c, 1, 1, entrada) >= 1)
            {
                char buffer[1024] = {0};
                pegaCodigo(raiz, c, buffer, 0);

                for (char *i = buffer; *i; i++)
                {
                    if (*i == '1')
                    {
                        aux = aux | (1 << (tamanho % 8));
                    }

                    tamanho++;

                    if (tamanho % 8 == 0)
                    {
                        fwrite(&aux, 1, 1, saida);
                        aux = 0;
                    }
                }
            }

            fwrite(&aux, 1, 1, saida);
            fseek(saida, 256 * sizeof(unsigned int), SEEK_SET);
            fwrite(&tamanho, 1, sizeof(unsigned), saida);

            fseek(entrada, 0L, SEEK_END);
            double tamanhoEntrada = ftell(entrada);

            fseek(saida, 0L, SEEK_END);
            double tamanhoSaida = ftell(saida);

            printf("\n\n---- TABELA DE CODIGOS ----\n");
            imprimeCodigo(raiz, "");
            printf("\n-----------------------------\n");
            LiberaArvore(raiz);

            printf("Arquivo de entrada: %s (%g caracteres)\nArquivo de saida: %s (%g caracteres)\n", arquivoEntrada, tamanhoEntrada / 1000, arquivoSaida, tamanhoSaida / 1000);
            printf("Taxa de compressao: %d%%\n", (int)((100 * tamanhoSaida) / tamanhoEntrada));

            fclose(entrada);
            fclose(saida);
        }
    }
}

/*
 *  Funcao que le o arquivo de Entrada e faz a descompress�o desse arquivo
 *  em um novo arquivo.
*/
void DescomprimeArquivo(const char *arquivoEntrada, const char *arquivoSaida)
{
    unsigned listaCaracteres[256] = {0};

    FILE *entrada = fopen(arquivoEntrada, "rb");
    if (entrada == NULL)
    {
        printf("Arquivo %s nao encontrado\n", arquivoEntrada);
    }
    else
    {
        FILE *saida = fopen(arquivoSaida, "wb");
        if (saida == NULL)
        {
            printf("\n\n*** ERRO AO CRIAR O ARQUIVO DE SAIDA! ***\n\n");
        }
        else
        {
            fread(listaCaracteres, 256, sizeof(listaCaracteres[0]), entrada);
            noh *raiz = CodigoDeHuffman(listaCaracteres);

            unsigned tamanho;
            fread(&tamanho, 1, sizeof(tamanho), entrada);

            unsigned posicao = 0;
            caractere aux = 0;

            while (posicao < tamanho)
            {
                noh *nodeAtual = raiz;

                while (nodeAtual->esq || nodeAtual->dir)
                {
                    nodeAtual = buscacaractere(entrada, posicao++, &aux) ? nodeAtual->dir : nodeAtual->esq;
                }

                fwrite(&(nodeAtual->c), 1, 1, saida);
            }

            LiberaArvore(raiz);

            fseek(entrada, 0L, SEEK_END);
            double tamanhoEntrada = ftell(entrada);

            fseek(saida, 0L, SEEK_END);
            double tamanhoSaida = ftell(saida);

            printf("Arquivo de entrada: %s (%g caracteres)\nArquivo de saida: %s (%g caracteres)\n", arquivoEntrada, tamanhoEntrada / 1000, arquivoSaida, tamanhoSaida / 1000);
            printf("Taxa de descompressao: %d%%\n", (int)((100 * tamanhoSaida) / tamanhoEntrada));

            fclose(saida);
            fclose(entrada);
        }
    }
}

int main()
{

    int op = 0;
    char nomeArquivo[100];
    char buffer[100];
    char nomeSaida[100];

    while (op != 3)
    {
        printf("\n*** MENU ***\n\n1- Comprimir Arquivo\n2- Descomprimir Arquivo\n3- Sair\n\nDigite a opcao: ");
        scanf("%d", &op);
        switch (op)
        {
        case 1:
            printf("\nDigite o nome do arquivo txt que deseja comprimir (EX: arquivo.txt): ");
            fflush(stdin);
            gets(nomeArquivo);

            printf("\nDigite o nome do arquivo de saida, sem a extencao do arquivo (EX: arquivosaida): ");
            fflush(stdin);
            gets(nomeSaida);
            strcat(nomeSaida, ".hx");

            ComprimeArquivo(nomeArquivo, nomeSaida);
            break;

        case 2:
            printf("\nDigite o nome do arquivo hx que deseja comprimir (EX: arquivo.hx): ");
            fflush(stdin);
            gets(nomeArquivo);

            printf("\nDigite o nome do arquivo txt que deseja criar ao descomprimir (EX: arquivo.txt): ");
            fflush(stdin);
            gets(nomeSaida);

            DescomprimeArquivo(nomeArquivo, nomeSaida);
            break;

        case 3:
            op = 3;
            printf("\n*** Saindo... ***\n");
            free(nomeArquivo);
            free(buffer);
            free(nomeSaida);
            break;

        default:
            printf("\n*** Digite uma opcao valida ***\n");
            break;
        }
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARQUIVO_SAVE "save.dat"
#define TOTAL_POKEMON 3

/*Dados do jogo*/

typedef struct {
    char nome[30];
    int nivel;
    int vida;
    int vidaMax;
    int ataque;
    int xp;
} Pokemon;

typedef struct {
    char nome[50];
    Pokemon pokemon;/*Pokemon do jogador*/
    int pocoes;
} Jogador;

/*Pokemons disponiveis (todos comecam no nivel 1).*/
/*Ordem: nome, nivel, vida, vidaMax, ataque, xp*/
const Pokemon POKEDEX[TOTAL_POKEMON] = {
    {"Charmander", 1, 45, 45, 12, 0},
    {"Squirtle",   1, 39, 39, 15, 0},
    {"Bulbasaur",  1, 44, 44, 11, 0}
};

/*Todas as função auxiliar*/

/*Le um numero digitado pelo usuario. Retorna -1 se nao for um numero.*/
int lerNumero(void)
{
    char linha[20];
    int numero;

    if (fgets(linha, sizeof(linha), stdin) == NULL) return -1;
    if (sscanf(linha, "%d", &numero) != 1) return -1;
    return numero;
}

/*Sobe 1 nivel: aumenta vida e ataque.*/
void aumentarNivel(Pokemon *p)
{
    p->nivel++;
    p->vidaMax += 5;
    p->vida = p->vidaMax;
    p->ataque += 2;
}

/*Soma XP. Cada nivel pede (nivel x 30) de XP.*/
void ganharXP(Pokemon *p, int quantidade)
{
    p->xp += quantidade;
    printf("%s ganhou %d de XP!\n", p->nome, quantidade);

    while (p->xp >= p->nivel * 30) {
        p->xp -= p->nivel * 30;
        aumentarNivel(p);
        printf(">>> %s subiu para o nivel %d!\n", p->nome, p->nivel);
    }
}

/*Salvar e carregar*/

/*Grava a struct do player de maneira inteira no arquivo. Retorna 1 se deu certo.*/
int salvarJogo(const Jogador *jogador)
{
    FILE *arquivo = fopen(ARQUIVO_SAVE, "wb");
    if (arquivo == NULL) return 0;

    fwrite(jogador, sizeof(Jogador), 1, arquivo);
    fclose(arquivo);
    return 1;
}

/*Le a struct Jogador do arquivo. Retorna 1 se deu certo.*/
int carregarJogo(Jogador *jogador)
{
    FILE *arquivo = fopen(ARQUIVO_SAVE, "rb");
    if (arquivo == NULL) return 0;

    int lido = fread(jogador, sizeof(Jogador), 1, arquivo);
    fclose(arquivo);
    return lido == 1;
}

/*Novo jogo (é literalmente só isso)*/

void novoJogo(Jogador *jogador)
{
    printf("\nDigite seu nome: ");
    fgets(jogador->nome, sizeof(jogador->nome), stdin);
    jogador->nome[strcspn(jogador->nome, "\n")] = '\0';/*tira o ENTER*/

    printf("\nEscolha seu Pokemon:\n");
    for (int i = 0; i < TOTAL_POKEMON; i++) {
        printf("%d - %s\n", i + 1, POKEDEX[i].nome);
    }

    int escolha = 0;
    while (escolha < 1 || escolha > TOTAL_POKEMON) {
        printf("Opcao: ");
        escolha = lerNumero();
    }

    jogador->pokemon = POKEDEX[escolha - 1];
    jogador->pocoes = 3;

    printf("\nVoce escolheu %s!\n", jogador->pokemon.nome);
}

/*Batalha do Jogo*/

void batalhar(Jogador *jogador)
{
    Pokemon *meu = &jogador->pokemon;

    /*Sorteia o inimigo e nivela com o do player. */
    Pokemon inimigo = POKEDEX[rand() % TOTAL_POKEMON];
    while (inimigo.nivel < meu->nivel) {
        aumentarNivel(&inimigo);
    }

    printf("\nUm %s selvagem (nivel %d) apareceu!\n", inimigo.nome, inimigo.nivel);

    while (1) {
        /*Mostra a situacao e pede a acao*/
        printf("\n%s: vida %d/%d | %s selvagem: vida %d/%d\n",
               meu->nome, meu->vida, meu->vidaMax,
               inimigo.nome, inimigo.vida, inimigo.vidaMax);
        printf("1 - Atacar | 2 - Pocao (%d) | 3 - Fugir\n", jogador->pocoes);
        printf("Acao: ");
        int acao = lerNumero();

        /*Turno do jogador*/
        if (acao == 1) {
            int dano = meu->ataque + rand() % 4;
            inimigo.vida -= dano;
            printf("Voce causou %d de dano!\n", dano);

        } else if (acao == 2) {
            if (jogador->pocoes <= 0) {
                printf("Voce nao tem pocoes!\n");
                continue;
            }
            jogador->pocoes--;
            meu->vida += meu->vidaMax / 4;/*cura 25%*/
            if (meu->vida > meu->vidaMax) meu->vida = meu->vidaMax;
            printf("Voce usou uma pocao!\n");

        } else if (acao == 3) {
            printf("Voce fugiu!\n");
            return;

        } else {
            printf("Opcao invalida!\n");
            continue;
        }

        /* Ve se o inimigo foi derrotado*/
        if (inimigo.vida <= 0) {
            printf("\nVoce venceu!\n");
            ganharXP(meu, inimigo.nivel * 20);

            if (rand() % 100 < 30) {/*30% de chance de ganhar a potion*/
                jogador->pocoes++;
                printf("Voce encontrou uma pocao!\n");
            }
            return;
        }

        /*Turno do inimigo*/
        int danoInimigo = inimigo.ataque + rand() % 4;
        meu->vida -= danoInimigo;
        printf("%s selvagem causou %d de dano!\n", inimigo.nome, danoInimigo);

        /*O jogador perdeu?*/
        if (meu->vida <= 0) {
            printf("\n%s desmaiou! Voce perdeu.\n", meu->nome);
            meu->vida = meu->vidaMax;/*Verifica se o pokemon morreu e regenera a vida dele*/
            return;
        }
    }
}

/*Principal parte do nosso jogo*/

int main(void)
{
    Jogador jogador;
    int jogoIniciado = 0;/* fica = 1 | depois de criar ou carregar um jogo */
    int opcao;

    srand((unsigned int)time(NULL));

    do {
        printf("\n===== MINI POKEMON =====\n");
        printf("1 - Novo jogo\n");
        printf("2 - Carregar jogo\n");
        printf("3 - Procurar Pokemon (batalhar)\n");
        printf("4 - Ver meu Pokemon\n");
        printf("5 - Salvar jogo\n");
        printf("0 - Sair\n");
        printf("Opcao: ");
        opcao = lerNumero();

        /*As opcoes 3, 4 e 5 so funcionam depois de iniciar um jogo.*/
        if (opcao >= 3 && opcao <= 5 && !jogoIniciado) {
            printf("\nComece um novo jogo ou carregue um save primeiro!\n");
            continue;
        }

        switch (opcao) {
            case 1:
                novoJogo(&jogador);
                jogoIniciado = 1;
                break;

            case 2:
                if (carregarJogo(&jogador)) {
                    printf("\nBem-vindo de volta, %s!\n", jogador.nome);
                    jogoIniciado = 1;
                } else {
                    printf("\nNenhum save encontrado.\n");
                }
                break;

            case 3:
                batalhar(&jogador);
                break;

            case 4:
                printf("\n%s | Nivel %d | Vida %d/%d | Ataque %d | XP %d/%d | Pocoes %d\n",
                       jogador.pokemon.nome, jogador.pokemon.nivel,
                       jogador.pokemon.vida, jogador.pokemon.vidaMax,
                       jogador.pokemon.ataque,
                       jogador.pokemon.xp, jogador.pokemon.nivel * 30,
                       jogador.pocoes);
                break;

            case 5:
                if (salvarJogo(&jogador)) printf("\nJogo salvo!\n");
                else                      printf("\nErro ao salvar.\n");
                break;

            case 0:
                printf("\nSaindo...\n");
                break;

            default:
                printf("\nOpcao invalida!\n");
        }
    } while (opcao != 0);

    return 0;
}

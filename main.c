#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//ESTRUTURAS
typedef struct {
	char nome[50];
	int vida;
	int vida_max;
	int ataque;
	int ouro;
	int pocoes;
} Jogador;

typedef struct {
	char nome[50];
	int vida;
	int vida_max;
	int ataque;
	int fase;
	int fases_totais;
	char frase[200];
	char frase_morte[200];
} Boss;

typedef struct {
	char nome[50];
	char dialogo[300];
	char dica[200];
} NPC;

typedef struct {
	char nome[50];
	int preco;
	int efeito;
} Item;

//FUNÇÕES
void mostrarVida(Jogador *j, Boss *b) {
	printf("\n  [Jogador] %s: %d/%d HP | ATQ: %d\n", j->nome, j->vida, j->vida_max, j->ataque);
	if (b != NULL) {
		printf("  [Boss]    %s (Fase %d/%d): %d/%d HP | ATQ: %d\n",
		       b->nome, b->fase, b->fases_totais, b->vida, b->vida_max, b->ataque);
	}
}

int combate(Jogador *j, Boss *b) {
	printf("\n========================================\n");
	printf("  *** BATALHA: %s ***\n", b->nome);
	printf("========================================\n");
	printf("\n  \"%s\"\n", b->frase);
	printf("  [Pressione ENTER para começar...]\n");
	getchar();

	while (j->vida > 0 && b->vida > 0) {
		mostrarVida(j, b);
		printf("\n  1) Atacar\n  2) Usar Poção (+50 HP)\n  3) Esquiva (70%% de chance + Contra-Ataque)\n");
		printf("  Escolha: ");
		int op;
		scanf("%d", &op);
		getchar();

		//Ação do jogador
		int esquiva_bem_sucedida = 0;

		if (op == 1) {
			int dano = j->ataque + (rand() % 5) - 2;
			if (dano < 1) dano = 1;
			b->vida -= dano;
			printf("  -> Você causou %d de dano em %s!\n", dano, b->nome);
		} else if (op == 2) {
			if (j->pocoes > 0) {
				j->pocoes--;
				j->vida += 50;
				if (j->vida > j->vida_max) j->vida = j->vida_max;
				printf("  -> Você usou uma poção! +50 HP (restam %d)\n", j->pocoes);
			} else {
				printf("  -> Sem poções!\n");
			}
		} else if (op == 3) {
			if (rand() % 10 < 7) {
				esquiva_bem_sucedida = 1;

				j->vida += 5;
				if (j->vida > j->vida_max) j->vida = j->vida_max;

				printf("  -> Você esquivou com perfeição e recuperou 5 HP!\n");

				int dano_esquiva = j->ataque + (rand() % 3) - 1;
				if (dano_esquiva < 1) dano_esquiva = 1;
				b->vida -= dano_esquiva;
				printf("  -> CONTRA-ATAQUE CRÍTICO! Você puniu o chefe causando %d de dano!\n", dano_esquiva);

			} else {
				int dano = b->ataque + (rand() % 4) - 1;
				if (dano < 1) dano = 1;
				j->vida -= dano;
				printf("  -> FALHOU! %s causou %d de dano!\n", b->nome, dano);
			}
		} else {
			printf("  -> Opção inválida!\n");
		}

		//Verificar mudança de fase
		if (b->vida <= 0 && b->fase < b->fases_totais) {
			b->fase++;
			b->vida = b->vida_max / b->fases_totais;
			b->ataque += 3;
			printf("\n  !!! %s TRANSFORMA! (Fase %d) !!!\n", b->nome, b->fase);
			printf("  O ar fica pesado... os olhos de %s brilham em vermelho.\n", b->nome);
			sleep(2);
		}

		//Ação do boss
		if (b->vida > 0 && j->vida > 0 && !esquiva_bem_sucedida) {
			int dano = b->ataque + (rand() % 6) - 2;
			if (dano < 1) dano = 1;
			j->vida -= dano;
			printf("  <- %s atacou! Você sofreu %d de dano.\n", b->nome, dano);
		}
	}

	if (j->vida <= 0) {
		printf("\n  *** VOCÊ MORREU ***\n");
		printf("  A escuridão engole seu corpo...\n");
		sleep(2);
		return 0;
	}

	printf("\n  *** %s FOI DERROTADO! ***\n", b->nome);
	printf("  \"%s\"\n", b->frase_morte);
	sleep(2);
	return 1;
}

void loja(Jogador *j) {
	Item itens[5];
	strcpy(itens[0].nome, "Poção de Vida");
	itens[0].preco = 15;
	itens[0].efeito = 50;
	strcpy(itens[1].nome, "Espada Enferrujada");
	itens[1].preco = 30;
	itens[1].efeito = 5;
	strcpy(itens[2].nome, "Armadura de Ferro");
	itens[2].preco = 50;
	itens[2].efeito = 30;
	strcpy(itens[3].nome, "Poção Grande");
	itens[3].preco = 40;
	itens[3].efeito = 100;
	strcpy(itens[4].nome, "Lâmina Sombria");
	itens[4].preco = 70;
	itens[4].efeito = 10;

	printf("\n  ===== LOJA DO VAGABUNDO (PROMOÇÃO!) =====\n");
	printf("  Ouro disponível: %d\n\n", j->ouro);
	for (int i = 0; i < 5; i++) {
		printf("  %d) %s - %d ouro (efeito: +%d)\n", i+1, itens[i].nome, itens[i].preco, itens[i].efeito);
	}
	printf("  0) Sair\n  Escolha: ");
	int op;
	scanf("%d", &op);
	getchar();

	if (op >= 1 && op <= 5) {
		if (j->ouro >= itens[op-1].preco) {
			j->ouro -= itens[op-1].preco;
			if (op == 1 || op == 4) {
				j->pocoes++;
				printf("  -> Você comprou %s! (+1 poção)\n", itens[op-1].nome);
			} else if (op == 2 || op == 5) {
				j->ataque += itens[op-1].efeito;
				printf("  -> Você comprou %s! (+%d ATQ)\n", itens[op-1].nome, itens[op-1].efeito);
			} else {
				j->vida_max += itens[op-1].efeito;
				j->vida += itens[op-1].efeito;
				printf("  -> Você comprou %s! (+%d HP MAX)\n", itens[op-1].nome, itens[op-1].efeito);
			}
		} else {
			printf("  -> Ouro insuficiente!\n");
		}
	}
}

void npc(Jogador *j, int qual) {
	NPC npcs[3];
	strcpy(npcs[0].nome, "Ancião Mordecai");
	strcpy(npcs[0].dialogo, "Ah... outro sobrevivente. Esta masmorra consome almas desde a Idade das Cinzas. Cuidado com o que habita as profundezas...");
	strcpy(npcs[0].dica, "Dica: Use a esquiva frequentemente! Ela agora te cura e abre brechas para golpes destrutivos.");

	strcpy(npcs[1].nome, "Vaga Lúmina");
	strcpy(npcs[1].dialogo, "Eu... eu era guardiã. Mas o Senhor das Somas me corrompeu. Fuja dele. Ele sente o medo...");
	strcpy(npcs[1].dica, "Dica: A loja reduziu os preços. Aproveite para estocar poções grandes.");

	strcpy(npcs[2].nome, "Mercador Sombra");
	strcpy(npcs[2].dialogo, "Ouro... eu aceito ouro. Mas saiba que cada moeda que gaste aqui, uma parte de sua alma fica presa a este lugar.");
	strcpy(npcs[2].dica, "Dica: A lâmina sombria custa menos agora e vai te dar uma força tremenda.");

	printf("\n  === %s ===\n", npcs[qual].nome);
	printf("  \"%s\"\n", npcs[qual].dialogo);
	printf("\n  [Sussurro] %s\n", npcs[qual].dica);
	printf("  [Pressione ENTER para continuar...]\n");
	getchar();
}

//MAIN
int main() {
	srand(time(NULL));
	int opcao1 = -1;

	//MENU
	while (opcao1 < 1 || opcao1 > 3) {
		printf("\n  !Masmorra do Terror!\n");
		printf("  1 : Iniciar\n");
		printf("  2 : Dificuldade\n");
		printf("  3 : Sair\n");
		printf("  Resposta: ");
		scanf("%d", &opcao1);
		getchar();
		if (opcao1 < 1 || opcao1 > 3) {
			printf("\n  Essa opção não existe.\n");
		}
	}

	//SAIR
	if (opcao1 == 3) {
		printf("\n  Saindo do jogo...\n");
		printf("  Cancelar? (1 para cancelar): ");
		scanf("%d", &opcao1);
		getchar();
		if (opcao1 == 1) {
			printf("  Continuando...\n");
			opcao1 = 1;
		} else {
			sleep(3);
			exit(0);
		}
	}

	//DIFICULDADE
	int dificuldade;
	if (opcao1 == 2) {
		printf("\n  1) Fácil\n  2) Normal\n  3) Pesadelo\n  Dificuldade: ");
		scanf("%d", &dificuldade);
		getchar();
	} else {
		dificuldade = 2;
	}

	//CRIAR JOGADOR
	Jogador jogador;
	printf("\n  Qual é o seu nome, herói? ");
	scanf("%s", jogador.nome);
	getchar();

	jogador.vida_max = 120;
	jogador.vida = 120;
	jogador.ataque = 15;
	jogador.ouro = 100;
	jogador.pocoes = 4;

	if (dificuldade == 1) {
		jogador.vida_max = 160;
		jogador.vida = 160;
		jogador.ataque = 20;
		jogador.ouro = 150;
	} else if (dificuldade == 3) {
		jogador.vida_max = 90;
		jogador.vida = 90;
		jogador.ataque = 11;
	}

	printf("\n  ================================\n");
	printf("  Bem-vindo(a), %s!\n", jogador.nome);
	printf("  Dificuldade: %s\n", dificuldade==1?"Fácil":dificuldade==2?"Normal":"Pesadelo");
	printf("  ================================\n");
	sleep(2);

	//ATO 1
	printf("\n  Você desce pelos corredores de pedra. O cheiro de cinzas é insuportável...\n");
	sleep(2);
	npc(&jogador, 0);

	//BOSS 1
	Boss boss1;
	strcpy(boss1.nome, "Guardião das Cinzas");
	boss1.vida_max = 100;
	boss1.vida = 100;
	boss1.ataque = 11;
	boss1.fase = 1;
	boss1.fases_totais = 2;
	strcpy(boss1.frase, "Você ousa pisar em meu domínio? As cinzas de mil heróis alimentam minha fúria!");
	strcpy(boss1.frase_morte, "As cinzas... retornam ao vazio...");

	if (dificuldade == 3) {
		boss1.vida_max = 140;
		boss1.vida = 140;
		boss1.ataque = 16;
	}

	if (!combate(&jogador, &boss1)) {
		printf("\n  GAME OVER. A Masmorra do Terror reclama mais uma alma.\n");
		return 0;
	}
	jogador.ouro += 60;
	printf("\n  [Você encontrou 60 ouro e 1 Poção de Vida!]\n");
	jogador.pocoes++;

	//ATO 2
	sleep(1);
	printf("\n  Um brilho fraco se move entre as sombras...\n");
	sleep(2);
	npc(&jogador, 1);

	//LOJA
	printf("\n  Um acampamento abandonado. Uma fogueira ainda queima.\n");
	printf("  Uma voz gutural: 'Compre, antes que a escuridão tome tudo...'\n");
	loja(&jogador);

	//BOSS 2
	Boss boss2;
	strcpy(boss2.nome, "A Lâmina Sanguinária");
	boss2.vida_max = 130;
	boss2.vida = 130;
	boss2.ataque = 14;
	boss2.fase = 1;
	boss2.fases_totais = 1;
	strcpy(boss2.frase, "Sangue... sinto o cheiro do seu sangue quente correndo nas veias!");
	strcpy(boss2.frase_morte, "Minha própria lâmina... me traiu...");

	if (dificuldade == 3) {
		boss2.vida_max = 180;
		boss2.vida = 180;
		boss2.ataque = 20;
	}

	if (!combate(&jogador, &boss2)) {
		printf("\n  GAME OVER. A Masmorra do Terror reclama mais uma alma.\n");
		return 0;
	}

	//FINAL DO JOGO
	printf("\n  ================================================\n");
	printf("  PARABÉNS! Você derrotou os males da Masmorra!\n");
	printf("  Você escapou com %d de ouro e sua alma intacta.\n", jogador.ouro);
	printf("  ================================================\n");

	return 0;
}

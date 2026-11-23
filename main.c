#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
	char nome[50];
	int vida;
	int vida_max;
	int ataque;
	int defesa;
	int ouro;
	int pocoes;
} Jogador;

typedef struct {
	char nome[50];
	int vida;
	int vida_max;
	int ataque;
	int defesa;
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

void mostrarVida(Jogador *j, Boss *b) {
	printf("\n  [Jogador] %s: %d/%d HP | ATQ: %d | DEF: %d\n", j->nome, j->vida, j->vida_max, j->ataque, j->defesa);
	if (b != NULL) {
		printf("  [Boss]    %s (Fase %d/%d): %d/%d HP | ATQ: %d | DEF: %d\n",
		       b->nome, b->fase, b->fases_totais, b->vida, b->vida_max, b->ataque, b->defesa);
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

		int esquiva_bem_sucedida = 0;

		if (op == 1) {
			int dano_base = j->ataque + (rand() % 5) - 2;
			int dano = dano_base - b->defesa;
			if (dano < 1) dano = 1;
			b->vida -= dano;
			printf("  -> Você causou %d de dano em %s! (Defesa de %s mitigou %d)\n", dano, b->nome, b->nome, b->defesa);
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

				int dano_esquiva_base = j->ataque + (rand() % 3) - 1;
				int dano_esquiva = dano_esquiva_base - b->defesa;
				if (dano_esquiva < 1) dano_esquiva = 1;
				b->vida -= dano_esquiva;
				printf("  -> CONTRA-ATAQUE CRÍTICO! Você puniu o chefe causando %d de dano!\n", dano_esquiva);

			} else {
				int dano_base = b->ataque + (rand() % 4) - 1;
				int dano = dano_base - j->defesa;
				if (dano < 1) dano = 1;
				j->vida -= dano;
				printf("  -> FALHOU! %s causou %d de dano! (Sua defesa mitigou %d)\n", b->nome, dano, j->defesa);
			}
		} else {
			printf("  -> Opção inválida!\n");
		}

		if (b->vida <= 0 && b->fase < b->fases_totais) {
			b->fase++;
			b->vida = b->vida_max / b->fases_totais;
			b->ataque += 3;
			b->defesa += 2;
			printf("\n  !!! %s TRANSFORMA! (Fase %d) !!!\n", b->nome, b->fase);
			printf("  O ar fica pesado... os olhos de %s brilham em vermelho e sua armadura enrijece.\n", b->nome);
			sleep(2);
		}

		if (b->vida > 0 && j->vida > 0 && !esquiva_bem_sucedida) {
			int dano_base = b->ataque + (rand() % 6) - 2;
			int dano = dano_base - j->defesa;
			if (dano < 1) dano = 1;
			j->vida -= dano;
			printf("  <- %s atacou! Você sofreu %d de dano. (Sua defesa mitigou %d)\n", b->nome, dano, j->defesa);
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
	itens[2].efeito = 4;
	strcpy(itens[3].nome, "Poção Grande");
	itens[3].preco = 40;
	itens[3].efeito = 100;
	strcpy(itens[4].nome, "Lâmina Sombria");
	itens[4].preco = 70;
	itens[4].efeito = 10;

	printf("\n  ===== LOJA DO VAGABUNDO (PROMOÇÃO!) =====\n");
	printf("  Ouro disponível: %d\n\n", j->ouro);
	for (int i = 0; i < 5; i++) {
		if (i == 2) {
			printf("  %d) %s - %d ouro (efeito: +%d DEF)\n", i+1, itens[i].nome, itens[i].preco, itens[i].efeito);
		} else {
			printf("  %d) %s - %d ouro (efeito: +%d %s)\n", i+1, itens[i].nome, itens[i].preco, itens[i].efeito, (i==0||i==3)?"HP":"ATQ");
		}
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
			} else if (op == 3) {
				j->defesa += itens[op-1].efeito;
				printf("  -> Você comprou %s! (+%d DEF)\n", itens[op-1].nome, itens[op-1].efeito);
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
	strcpy(npcs[1].dialogo, "Eu... eu era guardiã. Mas o Senhor das Sombras me corrompeu. Fuja dele. Ele sente o medo...");
	strcpy(npcs[1].dica, "Dica: Compre a Armadura de Ferro na fogueira para aumentar sua Defesa. O próximo oponente bate muito forte.");

	strcpy(npcs[2].nome, "Mercador Sombra");
	strcpy(npcs[2].dialogo, "Ouro... eu aceito ouro. Mas o verdadeiro perigo despertou abaixo de nós. O Senhor das Sombras não pode ser ferido facilmente.");
	strcpy(npcs[2].dica, "Dica: A defesa do Boss Final mitiga o seu dano base. Use poções com sabedoria.");

	printf("\n  === %s ===\n", npcs[qual].nome);
	printf("  \"%s\"\n", npcs[qual].dialogo);
	printf("\n  [Sussurro] %s\n", npcs[qual].dica);
	printf("  [Pressione ENTER para continuar...]\n");
	getchar();
}

int main() {
	srand(time(NULL));
	int opcao1 = -1;

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

	int dificuldade = 2;
	if (opcao1 == 2) {
		printf("\n  1) Fácil\n  2) Normal\n  3) Pesadelo\n  Dificuldade: ");
		scanf("%d", &dificuldade);
		getchar();
		opcao1 = 1;
	}

	Jogador jogador;
	printf("\n  Qual é o seu nome, herói? ");
	scanf("%s", jogador.nome);
	getchar();

	jogador.vida_max = 120;
	jogador.vida = 120;
	jogador.ataque = 15;
	jogador.defesa = 2;
	jogador.ouro = 100;
	jogador.pocoes = 4;

	if (dificuldade == 1) {
		jogador.vida_max = 160;
		jogador.vida = 160;
		jogador.ataque = 20;
		jogador.defesa = 4;
		jogador.ouro = 150;
	} else if (dificuldade == 3) {
		jogador.vida_max = 90;
		jogador.vida = 90;
		jogador.ataque = 11;
		jogador.defesa = 0;
	}

	printf("\n  ================================\n");
	printf("  Bem-vindo(a), %s!\n", jogador.nome);
	printf("  Dificuldade: %s\n", dificuldade==1?"Fácil":dificuldade==2?"Normal":"Pesadelo");
	printf("  ================================\n");
	sleep(2);

	printf("\n  Você desce pelos corredores de pedra. O cheiro de cinzas é insuportável...\n");
	sleep(2);
	npc(&jogador, 0);

	Boss boss1;
	strcpy(boss1.nome, "Guardião das Cinzas");
	boss1.vida_max = 100;
	boss1.vida = 100;
	boss1.ataque = 11;
	boss1.defesa = 1;
	boss1.fase = 1;
	boss1.fases_totais = 2;
	strcpy(boss1.frase, "Você ousa pisar em meu domínio? As cinzas de mil heróis alimentam minha fúria!");
	strcpy(boss1.frase_morte, "As cinzas... retornam ao vazio...");

	if (dificuldade == 3) {
		boss1.vida_max = 140;
		boss1.vida = 140;
		boss1.ataque = 16;
		boss1.defesa = 3;
	}

	if (!combate(&jogador, &boss1)) {
		printf("\n  GAME OVER. A Masmorra do Terror reclama mais uma alma.\n");
		return 0;
	}

	jogador.ouro += 60;
	jogador.pocoes++;

	sleep(1);
	printf("\n  Um brilho fraco se move entre as sombras...\n");
	sleep(2);
	npc(&jogador, 1);

	printf("\n  Um acampamento abandonado. Uma fogueira ainda queima.\n");
	printf("  Uma voz gutural: 'Compre, antes que a escuridão tome tudo...'\n");
	loja(&jogador);

	Boss boss2;
	strcpy(boss2.nome, "A Lâmina Sanguinária");
	boss2.vida_max = 130;
	boss2.vida = 130;
	boss2.ataque = 14;
	boss2.defesa = 3;
	boss2.fase = 1;
	boss2.fases_totais = 1;
	strcpy(boss2.frase, "Sangue... sinto o cheiro do seu sangue quente correndo nas veins!");
	strcpy(boss2.frase_morte, "Minha própria lâmina... me traiu...");

	if (dificuldade == 3) {
		boss2.vida_max = 180;
		boss2.vida = 180;
		boss2.ataque = 20;
		boss2.defesa = 5;
	}

	if (!combate(&jogador, &boss2)) {
		printf("\n  GAME OVER. A Masmorra do Terror reclama mais uma alma.\n");
		return 0;
	}

	jogador.ouro += 100;
	printf("\n  [Você derrotou a Lâmina! Encontrou +100 moedas de ouro no altar profanado.]\n");
	sleep(2);

	npc(&jogador, 2);

	printf("\n  Uma última chance de se preparar na fogueira moribunda...\n");
	loja(&jogador);

	Boss bossFinal;
	strcpy(bossFinal.nome, "O Senhor das Sombras");
	bossFinal.vida_max = 160;
	bossFinal.vida = 160;
	bossFinal.ataque = 19;
	bossFinal.defesa = 6;
	bossFinal.fase = 1;
	bossFinal.fases_totais = 2;

	strcpy(bossFinal.frase, "A escuridão é eterna, herói. Sua pequena chama se apagará agora!");
	strcpy(bossFinal.frase_morte, "Impossível... a luz... me consome...");
	if (dificuldade == 3) {
		bossFinal.vida_max = 220;
		bossFinal.vida = 220;
		bossFinal.ataque = 26;
		bossFinal.defesa = 10;
	}
	if (!combate(&jogador, &bossFinal)) {
		printf("\n  =============================================================\n");
		printf("  FIM DE JOGO: A ESCURIDÃO TRIUNFOU\n");
		printf("  =============================================================\n");
		printf("  O Senhor das Sombras ergue seu corpo inerte.\n");
		printf("  Seu nome, %s, será esquecido e apagado dos livros de história.\n", jogador.nome);
		printf("  A masmorra expande seus limites, consumindo o reino vizinho em trevas perpétuas.\n");
		printf("  Você falhou...\n");
		return 0;
	}
	printf("\n  =============================================================\n");
	printf("  PARABÉNS! VOCÊ DETONOU O SENHOR DAS SOMBRAS!\n");
	printf("  =============================================================\n");
	printf("  Com um golpe final, as correntes de sombras que prendiam o altar se quebram.\n");
	printf("  O teto da masmorra racha, deixando um feixe de luz solar iluminar o salão pela primeira vez.\n");
	printf("  %s purificou o núcleo corrompido e escapou carregando %d moedas de ouro.\n", jogador.nome, jogador.ouro);
	printf("  O reino está a salvo. Sua lenda acaba de começar!\n");
	return 0;
}

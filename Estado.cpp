#include "StdAfx.h"

int Estado::TEMPO_LIMITE = 0;

Estado::Estado()
{
}

Estado::Estado(int tempo_limite, string tabuleiro, char jogador, int jogadas) :
	tabuleiro(tabuleiro),
	jogador(jogador == 'A' ? 'B' : 'A'), // como o tabuleiro inicial vem do nosso adversario,
	adversario(jogador), // neste estado o adversario somos "nós"
	jogadas(jogadas),
	heuristica(0),
	profundidade(0),
	tamanhoTotal(tabuleiro.length())
{
	tamanhoLado = int(sqrt((float)tamanhoTotal));
	posicaoCentro = int(floor((float)tamanhoTotal/2));
	TEMPO_LIMITE = tempo_limite;
}

// copy
Estado::Estado(const Estado& outro) :
	tabuleiro(outro.tabuleiro),
	jogador(outro.jogador),
	adversario(outro.adversario),
	jogadas(outro.jogadas),
	heuristica(outro.heuristica),
	profundidade(outro.profundidade),
	tamanhoTotal(outro.tamanhoTotal),
	tamanhoLado(outro.tamanhoLado),
	posicaoCentro(outro.posicaoCentro)
{
}

Estado::~Estado(void)
{
}

// quando se criam sucessores, trocam-se os jogadores, o adversario
// passa a ser o jogador e vice-versa
void Estado::trocarJogadores()
{
	char _jogador = jogador;
	jogador = adversario;
	adversario = _jogador;
}

// verificar se o jogo terminou
bool Estado::jogoTerminado()
{
	return jogadas >= MAXIMO_JOGADAS ||
		(jogadas > 0 &&
			(tabuleiro.find(jogador) == string::npos ||
			tabuleiro.find(adversario) == string::npos)
		);
}

// verificar se uma coordenada está livre
bool Estado::livre(int coordenada)
{
	return tabuleiro[coordenada] == VAZIO;
}

// verificar se uma coordenada é o centro
bool Estado::centro(int coordenada)
{
	return coordenada == posicaoCentro;
}

// verificar se uma coordenada está encostada a uma parede
bool Estado::encostadoParede(int coordenada)
{
	return
		coordenada < tamanhoLado || coordenada >= tamanhoTotal - tamanhoLado || // cima e baixo
		coordenada % tamanhoLado == 0 || (coordenada + 1) % tamanhoLado == 0; // lados
}

// verificar se uma coordenada corresponde a um canto
bool Estado::canto(int coordenada)
{
	return
		coordenada == 0 || coordenada == tamanhoLado - 1 ||
		coordenada == tamanhoTotal - tamanhoLado || coordenada == tamanhoTotal - 1;
}

// contar as ocorrencias de peças de um determinado jogador adjacentes a
// uma coordenada
int Estado::contaAdjacentes(int coordenada, char jogadorAContar)
{
	int contagem = 0;
	if(coordenada > 0 && tabuleiro[coordenada - 1] == jogadorAContar)
		contagem++;
	if(coordenada < tamanhoTotal - 1 && tabuleiro[coordenada + 1] == jogadorAContar)
		contagem++;
	if(coordenada >= tamanhoLado && tabuleiro[coordenada - tamanhoLado] == jogadorAContar)
		contagem++;
	if(coordenada < tamanhoTotal - tamanhoLado && tabuleiro[coordenada + tamanhoLado] == jogadorAContar)
		contagem++;
	return contagem;
}

// conta o numero de peças de um determinado jogador
int Estado::contaPecas(char jogadorAContar)
{
	return count(tabuleiro.begin(), tabuleiro.end(), jogadorAContar);
}

// este método é chamado quando se gera um sucessor e se faz um movimento,
// e efetua as capturas possiveis da peça que se moveu para determinada posiçao
void Estado::capturarPecas(int posicao)
{
	// conta as peças capturadas para a contagem entrar
	// no valor heuristico
	int contagem = 0;

	// vetor para guardar as posiçoes a capturar
	vector<int> posicoes;

	// esquerda
	for(int i = posicao - 1;
		i >= 0 && (i == posicao - 1 || i % tamanhoLado != tamanhoLado - 1); i--)
	{
		// peças do adversario, adicionar ao vetor
		if(tabuleiro[i] == adversario)
			posicoes.push_back(i);
		// peça do jogador, efetuar as capturas
		else if(tabuleiro[i] == jogador)
		{
			for(int j = 0; j < posicoes.size(); j++)
			{
				tabuleiro[posicoes[j]] = VAZIO;
				contagem++;
			}
			break;
		}
		// se for vazio, sair
		else
			break;
	}
	// limpar as posiçoes encontradas
	posicoes.clear();
	//direita
	for(int i = posicao + 1;
		i < tamanhoTotal && (i == posicao + 1 || i % tamanhoLado != 0); i++)
	{
		// repete isto tudo para cada direçao, só muda a parte do ciclo for
		if(tabuleiro[i] == adversario)
			posicoes.push_back(i);
		else if(tabuleiro[i] == jogador)
		{
			for(int j = 0; j < posicoes.size(); j++)
			{
				tabuleiro[posicoes[j]] = VAZIO;
				contagem++;
			}
			break;
		}
		else
			break;
	}
	posicoes.clear();
	// cima
	for(int i = posicao - tamanhoLado; i >= 0 && i % tamanhoLado == posicao % tamanhoLado; i -= tamanhoLado)
	{
		if(tabuleiro[i] == adversario)
			posicoes.push_back(i);
		else if(tabuleiro[i] == jogador)
		{
			for(int j = 0; j < posicoes.size(); j++)
			{
				tabuleiro[posicoes[j]] = VAZIO;
				contagem++;
			}
			break;
		}
		else
			break;
	}
	posicoes.clear();
	//baixo
	for(int i = posicao + tamanhoLado; i < tamanhoTotal && i % tamanhoLado == posicao % tamanhoLado; i += tamanhoLado)
	{
		if(tabuleiro[i] == adversario)
			posicoes.push_back(i);
		else if(tabuleiro[i] == jogador)
		{
			for(int j = 0; j < posicoes.size(); j++)
			{
				tabuleiro[posicoes[j]] = VAZIO;
				contagem++;
			}
			break;
		}
		else
			break;
	}
	posicoes.clear();

	// somar a contagem de capturas à heuristica do estado
	heuristica += contagem * 50;
}

// calcula a heuristica deste estado
void Estado::calcularHeuristica()
{
	int pecasNossas = contaPecas(jogador);
	int pecasOutro = contaPecas(adversario);

	// durante a segunda fase, adicionar a diferença entre o numero de peças
	if(jogadas >= 0)
		heuristica += (pecasNossas - pecasOutro) * 25;

	// por cada peça
	for(int i = 0; i < tamanhoTotal; i++)
	{
		// fator positivo para o jogador, negativo para adversario e
		// posicoes vazias
		int fator = tabuleiro[i] == jogador ? 2 : -1;

		// cantos e paredes sao vantajosos nas duas fases
		if(canto(i))
			heuristica += 50 * fator;
		else if(encostadoParede(i))
			heuristica += 25 * fator;

		// durante a fase de jogo
		if(jogadas >= 0)
		{
			// contar adjacentes às peças do adversario
			if(tabuleiro[i] == adversario)
			{
				// se tiver das nossas, dá jeito para capturar
				heuristica += contaAdjacentes(i, jogador) * 25 * fator;
				// até podia dar jeito para fazer capturas de mais peças
				// de uma só vez, mas como o tabuleiro é relativamente
				// pequeno, dá mais jeito que estejam isoladas
				heuristica += contaAdjacentes(i, adversario) * 25 * -fator;
			}
		}
	}

	// caso o jogo tenha terminado, coloca-se o valor heuristico de
	// acordo com o exemplo
	if(jogoTerminado())
		heuristica = 1000000 *
			(pecasNossas - pecasOutro == 0 ?
				0 : // empate
					// jogador A primeiro, B segundo,
					// valor negativo para o derrotado
					pecasNossas > pecasOutro && jogador == 'A' ? 1 : -1);
}

// função para gerar sucessores do estado atual
void Estado::gerarSucessores()
{
	// fase de colocaçao das peças
	if(jogadas < 0)
	{
		// percorrer todas as posicoes
		for(int i = 0; i < tamanhoTotal; i++)
		{
			// a segunda, começar do fim
			for(int j = tamanhoTotal - 1; j >= 0; j--)
			{
				// como sao 2 peças, tem de ter posiçoes diferentes
				if(i != j)
				{
					if(livre(i) && livre(j) && !centro(i) && !centro(j))
					{
						// cópia do estado atual
						Estado e = Estado(*this);
						// aumenta o numero de jogadas
						e.jogadas = jogadas + 1;
						// quem era jogador agora passa a ser adversario
						e.trocarJogadores();
						// como se está a gerar a jogada seguinte, passa
						// a ser a vez do adversario a jogar
						e.tabuleiro[i] = adversario;
						e.tabuleiro[j] = adversario;
						// calcula-se a heuristica
						e.calcularHeuristica();
						// adiciona-se à priority_queue
						sucessores.push(e);
					}
				}
			}
		}
	}
	// fase de jogo
	else
	{
		// procura-se a próxima peça
		int posicao = tabuleiro.find(adversario);
		// enquanto houver mais peças, geram-se estados para cada um dos movimentos
		// possiveis da peça atual
		while(posicao != string::npos)
		{
			// mover para a esquerda
			if(posicao > 0 && posicao % tamanhoLado != 0 && tabuleiro[posicao - 1] == VAZIO)
			{
				// copia-se o estado atual
				Estado e = Estado(*this);
				// regista-se mais uma jogada
				e.jogadas = jogadas + 1;
				// trocam-se o jogador e o adversario
				e.trocarJogadores();
				// posicao anterior passa a estar vazia
				e.tabuleiro[posicao] = VAZIO;
				// posicao nova
				e.tabuleiro[posicao - 1] = adversario;
				// fazem-se as capturas deste movimento
				e.capturarPecas(posicao - 1);
				// calcula-se a heuristica
				e.calcularHeuristica();
				// adiciona-se à priority_queue
				sucessores.push(e);
			}
			// direita
			if(posicao < tamanhoTotal - 1 && posicao % tamanhoLado != tamanhoLado - 1 && tabuleiro[posicao + 1] == VAZIO)
			{
				// mesma coisa do anterior
				Estado e = Estado(*this);
				e.jogadas = jogadas + 1;
				e.trocarJogadores();
				e.tabuleiro[posicao] = VAZIO;
				e.tabuleiro[posicao + 1] = adversario;
				e.capturarPecas(posicao + 1);
				e.calcularHeuristica();
				sucessores.push(e);
			}
			// cima
			if(posicao >= tamanhoLado && tabuleiro[posicao - tamanhoLado] == VAZIO)
			{
				Estado e = Estado(*this);
				e.jogadas = jogadas + 1;
				e.trocarJogadores();
				e.tabuleiro[posicao] = VAZIO;
				e.tabuleiro[posicao - tamanhoLado] = adversario;
				e.capturarPecas(posicao - tamanhoLado);
				e.calcularHeuristica();
				sucessores.push(e);
			}
			// baixo
			if(posicao < tamanhoTotal - tamanhoLado && tabuleiro[posicao + tamanhoLado] == VAZIO)
			{
				Estado e = Estado(*this);
				e.jogadas = jogadas + 1;
				e.trocarJogadores();
				e.tabuleiro[posicao] = VAZIO;
				e.tabuleiro[posicao + tamanhoLado] = adversario;
				e.capturarPecas(posicao + tamanhoLado);
				e.calcularHeuristica();
				sucessores.push(e);
			}
			posicao = tabuleiro.find(adversario, posicao + 1);
		}
	}
}

// verifica-se se passou o tempo limite especificado
bool Estado::tempoTerminado()
{
	return clock() / CLOCKS_PER_SEC >= TEMPO_LIMITE;
}

// funçao negamax com alfa-beta pruning
int Estado::negamax(Estado& estado, int profundidade, int alfa, int beta, int jogador)
{
	// se atingir o limite de profundidade, ou terminar o tempo limite, ou o
	// jogo tenha chegado ao fim, retorna-se a heuristica, positiva ou negativa
	// dependendo do jogador (maximizante ou minimizante)
	if(profundidade == 0 || tempoTerminado() || jogoTerminado())
		return estado.heuristica * jogador;
	else
	{
		// geram-se os sucessores
		estado.gerarSucessores();
		while(!estado.sucessores.empty())
		{
			// proximo sucessor
			Estado sucessor = estado.sucessores.top();
			// marcar a profundidade no estado (para aparecer no cout)
			sucessor.profundidade = profundidade;
			// retira-se da lista de sucessores
			estado.sucessores.pop();
			// calcular o alfa, começa por ser -infinito, passa a ser o maximo entre
			// o valor atual e o resultado da negacao de uma chamada recursiva à
			// funçao negamax. como se nega também o indicador do jogador e os valores
			// do alfa e do beta, vai-se alternando entre maximizar e minimizar o resultado
			alfa = max(alfa, -negamax(sucessor, profundidade - 1, -beta, -alfa, -jogador));
			// se o alfa for maior, corta
			if(alfa >= beta)
				break;
		}
	}
	return alfa;
}

// devolve o melhor estado sucessor (profundidade inicial = 1 definida no ficheiro .h)
Estado Estado::melhorSucessor(int profundidade)
{
	// cópia do estado atual
	Estado melhorSucessor = Estado(*this);
	// jogadores trocam de papeis
	melhorSucessor.trocarJogadores();
	// inicializa-se o maximo com o minimo valor possivel
	int maximo = MENOS_INFINITO;

	// enquanto houver tempo vai-se aprofundando
	while(!tempoTerminado())
	{
		// geram-se as jogadas possiveis
		gerarSucessores();
		// por cada jogada
		while(!sucessores.empty())
		{
			// estado seguinte
			Estado sucessor = sucessores.top();
			// retira-se
			sucessores.pop();
			// calcula-se o valor maximo
			int valor = negamax(sucessor, profundidade, MENOS_INFINITO, MAIS_INFINITO, 1);
			// se for maior que o atual, guarda-se o sucessor como
			// sendo a melhor jogada possivel
			if (valor >= maximo)
			{
				melhorSucessor = sucessor;
				maximo = valor;
			}
		}
		// se ainda houver tempo aprofunda-se mais a procura
		profundidade++;
	}
	// regista-se a profundidade atingida para mostrar
	melhorSucessor.profundidade = profundidade;
	// retorna-se a melhor jogada
	return melhorSucessor;
}

// output para chamar o programa adversario
string Estado::output()
{
	stringstream s;
	s << tabuleiro << adversario << jogadas;
	return s.str();
}

// toString()
ostream& operator<<(ostream& out, Estado& estado)
{
	if(estado.profundidade < MAXIMO_JOGADAS)
	{
		out << endl << "Profundidade: " << estado.profundidade;
		out << " (excesso: " << float(clock()) / CLOCKS_PER_SEC - Estado::TEMPO_LIMITE << ")" << endl << endl;
	}

	int pecasA = estado.contaPecas('A');
	int pecasB = estado.contaPecas('B');
	for(int i = 0; i < estado.tamanhoTotal; i++)
	{
		out << " " << estado.tabuleiro[i] << " ";
		if(i > 0 && (i+1) % estado.tamanhoLado == 0)
			out << endl;
	}
	out << endl << "Movimentos: " << estado.jogadas << " (" << pecasA << "/" << pecasB << ") ";
	if(estado.heuristica != MENOS_INFINITO)
		out << estado.heuristica;
	out << endl;

	return out;
}
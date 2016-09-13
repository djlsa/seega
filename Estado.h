#pragma once

#define NUMERO_PECAS 12
#define MAXIMO_JOGADAS 100
#define MAIS_INFINITO numeric_limits<int>::max()
#define MENOS_INFINITO -MAIS_INFINITO
#define VAZIO '.'

class Estado
{
public:
	static int TEMPO_LIMITE;

	string tabuleiro;
	char jogador, adversario;
	int jogadas;
	int heuristica;
	int profundidade;
	int tamanhoTotal, tamanhoLado, posicaoCentro;

	// classe comparadora para passar à priority_queue
	struct ComparadorEstados
	{
		bool operator()(Estado& e1, Estado& e2) { return e1.heuristica > e2.heuristica; }
	};

	priority_queue<Estado, vector<Estado>, ComparadorEstados> sucessores;

	Estado();
	Estado(int tempo_limite, string tabuleiro, char jogador, int jogadas);
	Estado(const Estado& outro);
	~Estado(void);

	void trocarJogadores();

	bool tempoTerminado();
	bool jogoTerminado();
	bool livre(int coordenada);
	bool centro(int coordenada);
	bool encostadoParede(int coordenada);
	bool canto(int coordenada);
	int contaAdjacentes(int coordenada, char jogadorAContar);
	int contaPecas(char jogadorAContar);
	void capturarPecas(int posicao);

	void calcularHeuristica();
	void gerarSucessores();

	int negamax(Estado& estado, int profundidade, int alfa, int beta, int jogador);
	Estado melhorSucessor(int profundidade = 1);

	string output();

	// toString()
	friend ostream& operator<<(ostream& out, Estado& estado);
};

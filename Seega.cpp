// Seega.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main(int argc, char* argv[])
{
	cout << endl << endl << "[" << argv[0] << "] ";

	string argv2(argv[2]);
	string tabuleiro;
	tabuleiro = argv2.substr(0, 25);

	argv2 = argv2.substr(25);
	istringstream is(argv2);

	char jogador;
	is >> jogador;

	int jogadas;
	is >> dec >> jogadas;

	// inicializar o estado e gerar o melhor sucessor
	Estado e = Estado(atoi(argv[1]), tabuleiro, jogador, jogadas).melhorSucessor();
	// mostrar no ecra
	cout << endl << e;

	// escrever a linha a executar no ecrã (util para reproduzir um determinado passo)
	// cout << argv[3] << " " << argv[1] << " " << e.output().c_str() << " \"" << argv[0] << "\"" << endl;

	// nao consegui utilziar nenhum destes métodos
	//_execlp(argv[3], argv[1], e.output().c_str(), argv[0], 0);
	//_spawnl(_P_OVERLAY, argv[3], argv[1], e.output(), argv[0], 0);

	// colocar tudo numa string
	stringstream s;
	s << argv[3] << " " << argv[1] << " " << e.output() << " \"" << argv[0] << "\"";

	// correr o programa adversario
	if(!e.jogoTerminado())
		// esta funçao tem a desvantagem de deixar os processos pendurados em background
		// até o jogo terminar
		system(s.str().c_str());
	else
	{
		// mostrar o resultado quando o jogo terminar
		cout << "Resultado: " << e.heuristica << " (";
		cout << (jogador == 'A' ? argv[0] : argv[3]) << " vs " << (jogador == 'A' ? argv[3] : argv[0]) << ")";
		exit(EXIT_SUCCESS);
	}
}

## Estrutura Inicial do Código

Essa primeira parte do código funciona como uma base geral do projeto. Foi onde começamos a definir os principais elementos que seriam utilizados ao longo do desenvolvimento.

Para facilitar o processo e evitar erros de chamada entre funções, optamos por definir todas as variáveis globais logo no início. Isso ajuda a manter o código mais eficiente e organizado, além de tornar futuras manutenções mais simples.

Também definimos a estrutura Jogador, que armazena todas as informações importantes de cada jogador durante a partida, como nome, pontuação, número de vitórias, entre outros dados que serão utilizados durante o jogo.

Além disso, criamos os protótipos de todas as funções que seriam implementadas depois. Isso serve como uma espécie de planejamento, ajudando a visualizar quais partes do programa seriam necessárias e como elas se conectariam.


Agora vamos falar um pouco das funções que usamos no decorre do código 

# void limpar_buffer()

essa fução faz exatamente o que o nome sugere, limpa o buffer, evitando assim a leitura do "\n" da entrada do teclado.

# void inserir_nomes_jogadores()

Esta função é responsável por definir a quantidade de jogadores e registrar os nomes de cada um no início da partida. Isso torna o jogo mais personalizado e interativo, permitindo identificar cada jogador pelo nome ao longo das rodadas.

O programa solicita ao usuário a quantidade de jogadores (entre 2 e 6). Caso o valor digitado seja inválido, ele define automaticamente dois jogadores como padrão.

Em seguida, é feito um laço for para solicitar o nome de cada jogador individualmente, usando a função fgets().

Após a leitura do nome, a string é tratada para remover o caractere de nova linha (\n) ao final.

Por fim, todos os campos da estrutura Jogador são inicializados com valores padrão (zero ou ativo), preparando os jogadores para o início do jogo.

Essa função garante que todos os jogadores estejam corretamente configurados e prontos para participar da partida.


# int comprar_carta_probabilidade_alta()

Esta função é usada para gerar o valor de uma nova carta, com uma chance aumentada de sair uma carta alta (valores próximos de 10).

Inicialmente, ela gera um número aleatório de 1 a 10.

Depois, com uma probabilidade de 60% (quando rand() % 10 < 6), o valor da carta é substituído por um número entre 8 e 10.

O objetivo dessa função é simular um cenário onde o jogador tenha mais chances de tirar cartas altas, aumentando a tensão e o desafio na decisão de "comprar" ou não mais uma carta. Essa lógica influencia diretamente a estratégia de jogo dos participantes.

# void realizar_turno()

A função realizar_turno() é responsável por executar o turno de cada jogador na rodada atual do jogo. Assim como em muitos jogos de carta, cada jogador tem a chance de tomar decisões em seu próprio turno, o que contribui para o ritmo e equilíbrio da partida.

Como funciona:
A função percorre todos os jogadores através de um laço for.

Para cada jogador ativo (status == 1), é iniciado um laço while que permite ao jogador continuar comprando cartas enquanto sua pontuação for menor que o valor necessário para vencer (PONTOS_VITORIA).

O jogador é perguntado se deseja comprar mais uma carta. Se ele responder s ou S, uma nova carta é gerada pela função comprar_carta_probabilidade_alta() e adicionada à sua pontuação.

Após cada compra, o programa verifica:

Se a pontuação excedeu o limite (PONTOS_VITORIA), o jogador "estoura", seu status muda para inativo, e o contador de estouros é incrementado.

Se a pontuação for exatamente igual ao valor de vitória, é contabilizado como uma jogada perfeita (21 pontos exatos), e o jogador também é marcado como inativo.

Caso o jogador escolha não comprar mais cartas, seu turno termina e o status é alterado para inativo.

Essa função garante que todos os jogadores realizem seus turnos de forma justa, até que decidam parar ou ultrapassem o limite de pontos.


# int verificar_vencedor_rodada() 

 Após todos os jogadores concluírem seus turnos por meio da função realizar_turno(), é necessário identificar quem venceu a rodada — e essa é a função da verificar_vencedor_rodada().

Essa função percorre todos os jogadores e aplica a seguinte lógica:

Vitória direta: se algum jogador atingiu exatamente PONTOS_VITORIA (geralmente 21), ele é imediatamente considerado o vencedor.

Maior pontuação sem estourar: se ninguém atingiu os 21 pontos, vence quem tiver a maior pontuação abaixo desse limite.

Todos estouraram: se todos os jogadores ultrapassaram os 21 pontos, vence aquele que estourou por menos.

Empate: se houver empate em qualquer um dos cenários acima (mesma pontuação ou mesmo excesso), a função retorna -1, indicando que não houve um vencedor definido.

Essa lógica de verificação complementa perfeitamente a execução dos turnos feita anteriormente. Juntas, essas duas funções controlam o fluxo de jogadas e definem o resultado da rodada, formando o núcleo da mecânica do jogo.

# void mostrar_placar()

 A função mostrar_placar() é chamada ao final de todos os turnos de uma rodada para exibir a situação atual de cada jogador. Sua principal finalidade é informar o progresso da partida, facilitando a visualização do desempenho individual dos jogadores.

Funcionamento:
A função imprime a rodada e o jogo atual no topo do placar.

Em seguida, percorre todos os jogadores usando um laço for e exibe, para cada um:

O nome do jogador; A pontuação atual; O número de rodadas vencidas no jogo atual; O número de rodadas vencidas no total (acumulado entre os jogos); O número de jogos vencidos.

Esse feedback visual constante ajuda os jogadores a acompanharem seu desempenho e o dos adversários ao longo do tempo, criando uma experiência mais envolvente e competitiva.


# Funções resetar_rodada() e resetar_jogo()

Essas duas funções têm como objetivo reinicializar o estado do jogo em diferentes níveis, permitindo que ele continue de forma organizada entre rodadas e partidas.
Esta função é chamada ao final de uma rodada para preparar os jogadores para a próxima.

Zera os pontos dos jogadores (pontos = 0);

A função reseta_rodada(): Reativa todos os jogadores (status = 1), permitindo que participem novamente da próxima rodada. Com isso, a rodada seguinte começa com todos os jogadores em igualdade de condições, mantendo a continuidade do jogo.

A função resetar_jogo(): Já esta função é utilizada quando um novo jogo completo vai começar.

Zera o contador de vitórias por rodada no jogo atual (vitorias_rodada_jogo = 0) para todos os jogadores; Reinicia o número da rodada atual, colocando rodada_atual = 1.
Dessa forma, a função garante que todos os dados específicos da partida anterior sejam descartados, preservando apenas as estatísticas acumuladas, como vitórias totais de rodada e de jogo.


# void gerar_log_partida()

 Esta função é responsável por gerar um arquivo de texto (log_partida.txt) contendo um resumo completo dos resultados da partida.
No relatório, são registrados os desempenhos individuais dos jogadores ao longo do jogo, incluindo:

Total de rodadas vencidas por cada jogador; Quem obteve mais vezes a pontuação exata de 21; Quem teve mais estouros; Quem teve menos vitórias (indicando o jogador que mais perdeu).

Com essas informações, o arquivo evidencia o desempenho de cada participante e permite identificar, de forma clara, quem se destacou como vencedor da partida.


# Main()

 Esta função inicializa o jogo, executa a lógica principal das partidas e controla o fluxo entre rodadas, jogos e a decisão de encerrar ou continuar jogando.
Funcionalidades principais:
Inicializa o gerador de números aleatórios com `srand(time(NULL))`.
Solicita os nomes dos jogadores via `inserir_nomes_jogadores()`.
Controla um laço principal que permite a realização de até MAX_JOGOS partidas.
Para cada partida:
Inicia rodadas até que algum jogador vença o jogo atual.

Em cada rodada:
Distribui a primeira carta.
Realiza os turnos dos jogadores.
Verifica se restam jogadores ativos (com menos de 21 pontos).
Declara o vencedor da rodada ou empate.
Atualiza estatísticas de vitórias.
Finaliza o jogo caso um jogador atinja o número necessário de vitórias.
Caso um jogador atinja 3 jogos vencidos, gera o log final e pergunta se deseja continuar jogando.
Após o término da partida ou ao atingir o limite de jogos, gera o arquivo de log da partida.
Exibe mensagem de finalização ao usuário.

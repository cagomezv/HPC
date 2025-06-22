Board board;
// Initializes the board. A vector of weights is passed. 
void board.init(N,weights);
// Sets the the state of a cell. 
void board.setv(i,j,state);
// rescatter the board according to new weights
void board.re_scatter(new_weights);
// Advances the board one generation.
void board.advance();
// Prints the board
void print();
// Print statistics about estiamted processing rates
void board.statistics();
// Reset statistics
void board.reset_statistics();

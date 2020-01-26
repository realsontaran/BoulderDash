void init_game(char *title, int xpos, int ypos, int width, int height );
void handle_events();

void update();
void render();
void clean();
void showGameBar(char *t, char *diamond_count, char *life_count, char *game_score );

void set_miner();
void move_rockR();
void move_rockL();
void moveR();
void moveL();
void moveU();
void moveD();
void CollectDiamond();
void walkSounds();
void nextlevel();


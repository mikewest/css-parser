typedef struct Node {
    Token       *value;
    struct Node *right;
    struct Node *left;
    struct Node *parent;
} ParseTree;

ParseTree * generate_parse_tree( Token* *list );

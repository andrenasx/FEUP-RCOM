// Frame Macros

#define FLAG 0x7e  // Flag de inicio e fim
#define ESCAPE 0x7d // Octeto de escape
#define STUFFING 0x20 // Octeto para stuffing

#define A_ER 0x03  // Campo de Endereço (A) de commandos do Emissor, resposta do Receptor
#define A_RE 0x01  // Campo de Endereço (A) de commandos do Receptor, resposta do Emissor

#define C_SET 0x03 // Campo de Controlo - SET (set up)
#define C_DISC 0x0b // Campo de Controlo - DISC (disconnect)
#define C_UA 0x07 // Campo de Controlo - UA (Unnumbered Acknowledgement)
#define C_RR0 0x05 // Campo de Controlo - RR (receiver ready / positive ACK))
#define C_RR1 0x85
#define C_REJ0 0x01 // Campo de Controlo - REJ (reject / negative ACK))
#define C_REJ1 0x81
#define C_I0 0x00 // Campo de Controlo - I
#define C_I1 0x40

#define BCC(a,c) (a ^ c)

#define VERIFY_C(c) (c == C_SET || c == C_DISC || c == C_UA || c == C_RR0 || c == C_RR1 || c == C_REJ0 || c == C_REJ1) ? 1 : 0

#define MAX_SIZE 256
// 4.2.1 Section - https://www.w3.org/Protocols/rfc959/4_FileTransfer.html

#define COMMAND_OKAY_CODE       200 //Command okay.
#define SERVICE_READY_CODE      220 //Service ready for new user.
#define SERVICE_CLOSING_CODE    221 //Service closing control connection.
#define NEED_PASSWORD_CODE      331 //User name okay, need password.
#define USER_LOGGED_IN_CODE     230 //User logged in, proceed.
#define CWD_OKAY_CODE           250 //Requested file action okay, completed.
#define ENTER_PASSIVE_MODE_CODE 227 //Entering Passive Mode (h1,h2,h3,h4,p1,p2).
#define RETR_FILE_OKAY_CODE     150 //File status okay; about to open data connection.
#define RETR_TRANSF_START_CODE  125 //Data connection already open; transfer starting.
#define RETR_SUCCESS_CODE       226 //Closing data connection. Requested file action successful
#define ACTION_FAILED_CODE      550 //Requested action not taken. File unavailable (e.g., file not found, no access).

// Command Terminator
#define CRLF "\r\n"
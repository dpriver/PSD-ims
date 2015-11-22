#include <stdio.h>
#include <mysql.h>
#include "persistence.h"


int main(int argc, char **argv){

	MYSQL *bd;

	if (argc < 3) {
		printf("Usage: %s <bd_user> <bd_pass>\n", argv[0]);
		return -1;
	}	

	// Init environment
  //bd = (MYSQL *)init_bd(argv[1], argv[2] ,"PSD");
  
  //add_user(bd,10,"pepe","contrasena","information asdasdd asdasdaa sdaaasdas");
  //add_user(bd,2,"antonio","information");
  //del_user(bd,"pepe");
 //printf("%d\n",exist_user(bd,"pe12312pe2"));
  //accept_friend(bd,1,2);
  //send_request(bd,1,2);
  //accept_friend(bd,1,2);
  //refuse_request(bd,1,2);
  //del_friends(bd,1,2); 
   //get_id_user(bd,"antonio");
  //add_chat(bd,2,2,"informarcion");
  //del_chat(bd,1);
  //add_user_chat(bd,4,1);
  //del_user_chat(bd,2,1);
  //printf("%d\n",get_cont(bd));
  //sum_cont(bd);
	return 0;
}

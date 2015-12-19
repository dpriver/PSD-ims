#include <stdio.h>
#include "persistence.h"
#include "soapH.h"


// To instanciate psdims__ structs the functions soap_malloc(<soap>, <size>) must be used
// instead of malloc(<size>), si a soap struct is needed in order to use persistence.... 
// a complete MESS

// All this can not be used without an initialized soap struct

/*
void test_list_friends(persistence *persistence){
	int i=0;
	psdims__user_list *friends=malloc(sizeof(psdims__user_list));

	get_list_friends(persistence,2,0,friends);

	while(i<friends->__sizenelems){
		printf("%s ,  %s\n",friends->user[i].name,friends->user[i].information);
		i++;
	}
}

void test_list_chats(persistence *persistence){
	int i=0,j;
	psdims__chat_list *chats=malloc(sizeof(psdims__chat_list));

	get_list_chats(persistence,2,0, chats);

	while(i<chats->__sizenelems){
		printf("Number chat=> %d, Admin chat=> %s , Description=> %s \n",chats->chat_info[i].chat_id,chats->chat_info[i].admin,chats->chat_info[i].description);
		for(j=0;j<chats->chat_info[i].members->__sizenelems;j++){
			printf("Miembro=> %s \n", chats->chat_info[i].members->name[j].string);
			}
		i++;
	}
}

void test_get_messages_request(persistence *persistence,int id_user,int id_chat){
	int i=0;
	psdims__message_list *messages=malloc(sizeof(psdims__message_list));

	if(exist_user_in_chat(persistence,id_user,id_chat)!=1){
		printf("No exite el usuario en ese chat\n");
		exit(1);
	}
		

	get_list_messages(persistence,id_chat,21,messages);

	while(i<messages->__sizenelems){
		printf("%s ,  %s ,  %d\n",messages->messages[i].user,messages->messages[i].text,messages->messages[i].send_date);
		i++;
	}
}

void test_send_request(persistence *persistence,int id_user,int id_request_name){
	int i=0;
	psdims__message_list *messages=malloc(sizeof(psdims__message_list));

	if(exist_friendly(persistence,id_user,id_request_name)!=0){
		printf("Ya son amigos\n");
		exit(1);
	}

	if(exist_request(persistence,id_user,id_request_name)!=0){
		printf("Ya existe una petición de amistad\n");
		exit(1);
	}

	send_request(persistence,id_user, id_request_name,0);
}

void test_accept_request(persistence *persistence,int id_user,int id_request_name){
	int i=0;
	psdims__message_list *messages=malloc(sizeof(psdims__message_list));

	if(exist_request(persistence,id_user,id_request_name)==0){
		printf("No existe una petición de amistad\n");
		exit(1);
	}
    
    accept_friend_request(persistence,id_user,id_request_name,0);
}

void test_decline_request(persistence *persistence,int id_user,int id_request_name){
	int i=0;
	psdims__message_list *messages=malloc(sizeof(psdims__message_list));

	if(exist_request(persistence,id_user,id_request_name)==0){
		printf("No existe una petición de amistad\n");
		exit(1);
	}
    
    decline_friend_request(persistence,id_user,id_request_name);
}

*/
void test_quit_from_chat(persistence *persistence,int id_user,int chat_id){
	int first_user;

	if(chat_exist(persistence,chat_id)!=1)
		exit(-1);

	if(exist_user_in_chat(persistence,id_user,chat_id)!=1)
		exit(-1);

	if(del_user_chat(persistence,id_user,chat_id)!=0)	
		exit(-1);

	if(still_users_in_chat(persistence,chat_id)==1){
		if(is_admin(persistence,id_user,chat_id)==1){
			if((first_user=get_first_users_in_chat(persistence,chat_id))==1)
				exit(-1);
			if(change_admin(persistence,first_user,chat_id)==1)
				exit(-1);
		}
	}
	else{
		if(del_chat(persistence,chat_id)!=0)	
			exit(-1);
	}
}

void test_add_member_chat(persistence *persistence,int id_user,int chat_id){
	int first_user;

	if(chat_exist(persistence,chat_id)!=1)
		return exit(-1);

    if(exist_user_in_chat(persistence,id_user,chat_id)==1){
		printf("Ya existe el usuario en el chat");
		exit(-1);
	}

	if(add_user_chat(persistence,id_user,chat_id,0)!=0)
		exit(-1);
}

void test_add_chat(persistence *persistence,int admin_id){
	int id_user;

	if(add_chat(persistence, admin_id, "Descripción de prueba",0,0)!=0)
		exit(-1);

}

void test_notifacitions(persistence *persistence,int user_id){	
	int i=0,j=0;

	struct soap *soap = malloc(sizeof(struct soap));
	psdims__notifications *notifications = malloc(sizeof(psdims__notifications));

	get_notifications(persistence,user_id,0,soap,notifications);

	printf("------List friends request-------\n");

	for(i;i<notifications->friend_request.__sizenelems;i++){
		printf("Friend request- %s  ",notifications->friend_request.user[i].name.string);
		printf("Send date- %d\n",notifications->friend_request.user[i].send_date);
	}	

	printf("------List messages request-------\n");

	for(j;j<notifications->chats_with_messages.__sizenelems;j++){
		printf("Chat with messages %d\n",notifications->chats_with_messages.chat[j].chat_id);
	}	

	printf("Timestamp %d\n",notifications->last_timestamp);	
}

int main(int argc, char **argv){
	persistence *persistence=init_persistence(argv[1],argv[2]);	

	//test_get_messages_request(persistence,3,3);
	
	//test_list_chats(persistence);

	//test_list_friends(persistence);

	//test_send_request(persistence,1,4);

	//test_accept_request(persistence,1,4);

	//test_decline_request(persistence,4,3);

	//test_quit_from_chat(persistence,3,3);
	
	//test_add_member_chat(persistence,4,2);

	//test_add_chat(persistence,4);

	//free_persistence(persistence);

	test_notifacitions(persistence,3);

	return 0;
}

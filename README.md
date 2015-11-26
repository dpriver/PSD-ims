# Project "IMS" for the subject "Distributed Systems Programming"

## Design decisions


> 1. Client aplications only include the client api "psd_ims_client.h"
>
> 2. Server aplications only include the server api "psd_ims_server.h"
>
> 3. Messages have a timestamp "send_date" assigned by the server, so the clients send the messages, and
> the server return OK and the timestamp
>
> 4. friend request have timestamp too.
>
> 5. To get a chat's messages, the client send to the server the chat id and the timestamp
> of the last message it has
>
> 6. If the client has to syncronize its friend list, or chat list, as that is a non-common operation
> it has to ask the server for the full list, and then update its own.


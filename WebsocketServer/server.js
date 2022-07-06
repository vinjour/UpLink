
const WebSocket = require('ws');

const PORT = 8000;

const wsServer = new WebSocket.Server({
    port: PORT
});

wsServer.on('connection', function(socket){
    console.log("A client just connected");
    socket.on('message', function(msg){
        console.log("Received message from client : " + msg);
        socket.send(msg);
    });
});

console.log((new Date()) + " Server is listening on port " + PORT);

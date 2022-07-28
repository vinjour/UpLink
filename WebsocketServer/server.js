const WebSocket = require('ws');
const PORT = 8002;
const wsServer = new WebSocket.Server({
    port: PORT
});
wsServer.on('connection', function(socket){
    console.log("\nA router just connected");
    socket.on('message', function(data){
        var msg = data.toString('utf8');
        const obj = JSON.parse(msg);
        console.log(msg);
        switch(obj.action)
        {
            case 'router':
                console.log("Creating a new router");
                socket.send(JSON.stringify({action: new String("router"), mac: obj.mac}));
                //createRouter().catch(console.error).finally();
                break;
            case 'connect':
                console.log("Connecting a client");
                socket.send(JSON.stringify({action: new String("accept"), name: obj.name, mac: obj.mac}));
                //const name = obj.name;
                //createClient(name).catch(console.error).finally();
                break;
            case 'transfer':
                console.log("Transfering funds");
                //const origin = obj.nameOrigin;
                //transfer(origin).catch(console.error).finally();
                break;
            case 'delete':
                console.log("Deleting a client");
                socket.send(JSON.stringify({action: new String("delete"), name: obj.name, mac: obj.mac}));
                break;
            default:
                console.log("Unknown command received");
        }
    });
});
console.log((new Date()) + "\nServer is listening on port " + PORT);
wsServer.on('error', err => { console.log(err) });
wsServer.on('close', () => { console.log('close') });
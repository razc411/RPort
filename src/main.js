var net = require('net');
var configFile = "rport.cfg";

function parse_config()
{
    var fs = require('fs');

    var configuration = JSON.parse(
        fs.readFileSync(configFile)
    );

    console.log(configuration.fromPort);
    console.log(configuration.toPort);
    console.log(configuration.toAddress);
}

function forward(sourcePort, desitnationPort, destinationIP){

    net.createServer(function(s)
    {
        var buffer = "";
        var connected = false;
        var connection = net.createConnection(destinationPort);
        s.on('data', function(outgoing) {
            if (connected)
            {
               connection.write(outgoing);
            } 
            
            else 
            {
               buff += outgoing.toString();
            }
        });
        connection.on('connect', function() {
            connected = true;
            cli.write(buff);
        });
        cli.pipe(s);
    }).listen(sourcePort);
}

parse_config()
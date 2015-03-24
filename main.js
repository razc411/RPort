/**
*   RPort Port Forwarder
*   By Ramzi Chennafi
*   A Simple port forwarder written in node js. rport.cfg is the config file 
*   to change if you want new rules added. 
*/
var net = require('net');
var configFile = "rport.cfg";
var fs = require('fs');
/**
*   Function:   forward(sourcePort, destinationPort, destinationIP)
*   Author:     Ramzi Chennafi
*   Date:       March 15 2015
*   Returns:    Nothing
*
*   Notes
*   Opens a listen connection on the specified source port. Routes any data
*   from that source port to the destination port and destination IP.
*       sourcePort - the port to listen to for incoming connections
*       destinationPort - the port to send the connection to
*       destinationIP - the ip to send the connection to
*/
function forward(sourcePort, destinationPort, destinationIP)
{
    net.createServer(function(s)
    {
        var buffer = "";
        var connected = false;
        var sendTo = net.createConnection({
                        host : destinationIP, 
                        port : destinationPort});

        s.on('data', function(outgoing) {
            if (connected)
            {
               sendTo.write(outgoing);
            } 
            else 
            {
               buffer += outgoing.toString();
            }
        });

        sendTo.on('connect', function() {
            connected = true;
            sendTo.write(buffer);
        });
        sendTo.pipe(s);
    }).listen(sourcePort);
}
/**
*   Function:   Main Program Loop
*   Author:     Ramzi Chennafi
*   Date:       March 15 2015
*   Returns:    Nothing
*
*   Notes
*   Reads in the rport.cfg file and creates fowarding rules for each one.
*   Then sets them in motion to listen to incoming connections.
*/
var rules = JSON.parse(fs.readFileSync(configFile, 'utf8'));
console.log("Rules Added to RPort");

for(var i = 0; i < rules.length; i++) {
    console.log(rules[i]);
    forward(rules[i].fromPort, rules[i].toPort, rules[i].toAddress);
}

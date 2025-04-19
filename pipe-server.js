const net = require('net');
const pipeName = '\\\\.\\pipe\\TestPipe';

const client = net.createConnection(pipeName, () => {
    console.log('[+] Connected to pipe server');

    client.write('Hello from Web UI!');
});

client.on('data', (data) => {
    console.log('[Server] ' + data.toString());
});

client.on('end', () => {
    console.log('[+] Disconnected from pipe server');
});

client.on('error', (err) => {
    console.error('[-] Pipe error:', err.message);
});

const net = require('net');
const http = require('http');
const pipeName = '\\\\.\\pipe\\TestPipe';

const client = net.createConnection(pipeName, () => {
    console.log('[+] Connected to pipe server');

    client.write('Hello from Web UI!');
});

const server = http.createServer((req, res) => {
    // Routing
    if (req.url === '/') {
        if (req.method === 'POST') {
            res.writeHead(200, { 'Content-Type': 'text/plain' });
            res.end('Received POST request!');
            client.write('786');
        } else {
            res.writeHead(200, { 'Content-Type': 'text/plain' });
            res.end('Basic server running!');
        }
    } else if (req.url === '/about') {
        res.writeHead(200, { 'Content-Type': 'text/plain' });
        res.end('About page');
    } else {
        res.writeHead(404, { 'Content-Type': 'text/plain' });
        res.end('Page not found');
    }


});

const port = 3000;

server.listen(port, () => {
    console.log(`Server listening on port ${port}`);
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

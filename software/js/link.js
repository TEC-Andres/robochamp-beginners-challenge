const Link = (() => {
// Firmware link: the only module that talks to the robot (WebSocket on port 81, text protocol).
//   software -> firmware: D,y,w | S | M,idx,v | G,fl,fr,rl,rr | SAVE | C | L,colorIdx | V,degrees
//   firmware -> software: G,fl,fr,rl,rr | C,colorIdx | L,0/1
// Events: 'status' (bool), 'gains' (number[4]), 'color' (index), 'learned' (bool)

const WS_PORT = 81; // keep in sync with Constants::Network::wsPort in firmware
// opened from disk there is no hostname: fall back to the ESP32 access point address, or pass ?host=<ip>
const HOST = new URLSearchParams(location.search).get('host') || location.hostname || '192.168.4.1';
let ws;
const handlers = {};

const on = (type, fn) => (handlers[type] ||= []).push(fn);
const emit = (type, value) => (handlers[type] || []).forEach(fn => fn(value));

function send(msg) {
  if (ws && ws.readyState === 1) ws.send(msg);
}

const robot = {
  drive: (y, w) => send(`D,${y.toFixed(2)},${w.toFixed(2)}`),
  stop: () => send('S'),
  testMotor: (i, v) => send(`M,${i},${v}`),
  setGains: g => send('G,' + g.map(x => (+x).toFixed(2)).join(',')),
  save: () => send('SAVE'),
  readColor: () => send('C'),
  learn: i => send('L,' + i),
  servo: deg => send('V,' + Math.round(deg)), // 0..180
};

function onMessage(msg) {
  const p = msg.split(',');
  if (p[0] === 'G') emit('gains', p.slice(1).map(Number));
  else if (p[0] === 'C') emit('color', +p[1]);
  else if (p[0] === 'L') emit('learned', p[1] === '1');
}

function connect() {
  ws = new WebSocket(`ws://${HOST}:${WS_PORT}`);
  ws.onopen = () => emit('status', true);
  ws.onclose = () => { emit('status', false); setTimeout(connect, 1000); };
  ws.onmessage = e => onMessage(e.data);
}

return { on, robot, connect };
})();

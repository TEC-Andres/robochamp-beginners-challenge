const Cal = (() => {
// Calibration overlay: motor gains + color-sensor learning. Talks to the robot only through link.js.

const COLORS = ['Red', 'Yellow', 'Green', 'Blue', 'Gray'];
const MOTORS = ['Front left', 'Front right', 'Rear left', 'Rear right'];
const TEST_SPEED = 0.4;
const TEST_REFRESH_MS = 200; // robot stops after 500 ms of silence

const isOpen = () => $('cal').classList.contains('open');
function open() { Link.robot.stop(); $('cal').classList.add('open'); }
function close() { $('cal').classList.remove('open'); }

const gainEls = [];
let testTimer = null;
const stopTesting = () => { clearInterval(testTimer); testTimer = null; };
const sendGains = () => Link.robot.setGains(gainEls.map(g => g.range.value));
function setGainUI(vals) {
  vals.slice(0, 4).forEach((v, i) => { gainEls[i].range.value = v; gainEls[i].out.textContent = v.toFixed(2); });
}

function init() {
  MOTORS.forEach((name, i) => {
    const row = document.createElement('div');
    row.className = 'row';
    row.innerHTML = `<label>${name}</label><input type="range" min="0.5" max="1.5" step="0.01" value="1"><output>1.00</output>
      <button class="chip">Test</button>`;
    const range = row.querySelector('input'), out = row.querySelector('output'), test = row.querySelector('button');
    range.addEventListener('input', () => { out.textContent = (+range.value).toFixed(2); sendGains(); });
    test.addEventListener('pointerdown', e => {
      test.setPointerCapture(e.pointerId); test.classList.add('on');
      Link.robot.testMotor(i, TEST_SPEED);
      stopTesting(); testTimer = setInterval(() => Link.robot.testMotor(i, TEST_SPEED), TEST_REFRESH_MS);
    });
    ['pointerup', 'pointercancel'].forEach(t => test.addEventListener(t, () => { test.classList.remove('on'); stopTesting(); Link.robot.stop(); }));
    $('gains').appendChild(row);
    gainEls.push({ range, out });
  });

  COLORS.forEach((name, i) => {
    const b = document.createElement('button');
    b.className = 'chip'; b.textContent = 'Learn ' + name;
    b.onclick = () => Link.robot.learn(i);
    $('learn').appendChild(b);
  });

  $('save').onclick = () => { Link.robot.save(); $('calnote').textContent = 'Gains saved on the robot.'; };
  $('reset').onclick = () => { setGainUI([1, 1, 1, 1]); sendGains(); };
  $('read').onclick = Link.robot.readColor;
  $('close').onclick = close;

  Link.on('gains', setGainUI);
  Link.on('learned', ok => { $('calnote').textContent = ok ? 'Learned.' : 'Sensor read nothing — check wiring and light.'; });
}

return { COLORS, isOpen, open, close, init };
})();

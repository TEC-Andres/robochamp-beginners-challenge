// Wires input, firmware link and calibration panel together.

const SEND_MS = 40; // 25 Hz; robot stops after 500 ms of silence
const BAR = ['#ff3b3b', '#ffd23b', '#3ddc84', '#3b82ff', '#9aa3b2']; // light bar mirrors the sensor reading

// ---------- firmware -> UI ----------
Link.on('status', up => {
  $('dot').className = up ? 'dot ok' : 'dot';
  $('conn').textContent = up ? 'Connected' : 'Reconnecting…';
});
Link.on('color', i => {
  $('color').textContent = 'Color: ' + Cal.COLORS[i];
  $('bar').style.background = BAR[i];
  $('bar').style.boxShadow = `0 0 1.6vh ${BAR[i]}`;
});

// ---------- UI -> firmware ----------
const SERVO_CLOSED = 0, SERVO_OPEN = 90; // closed = Constants::ServoConfig::startDeg (boot position)
let servoOpen = false;
const toggleServo = () => { servoOpen = !servoOpen; Link.robot.servo(servoOpen ? SERVO_OPEN : SERVO_CLOSED); };
const actions = { tri: toggleServo }; // △ opens / closes the servo
Input.bindTouch(actions);
Cal.init();

let wasMoving = false, lastSendAt = 0;
setInterval(() => {
  $('pads').textContent = Input.pollGamepad(actions);
  if (Cal.isOpen()) return;                                    // calibration screen owns the motors
  const [y, w] = Input.command();
  const moving = y !== 0 || w !== 0;
  const now = performance.now();
  // idle: send S once, then stay quiet. moving: stream so the failsafe stays fed.
  if (moving || wasMoving || now - lastSendAt > 1000) { moving ? Link.robot.drive(y, w) : Link.robot.stop(); lastSendAt = now; }
  wasMoving = moving;
}, SEND_MS);

document.addEventListener('visibilitychange', () => { if (document.hidden) Link.robot.stop(); });
window.addEventListener('blur', Link.robot.stop);

Link.connect();
if (new URLSearchParams(location.search).has('cal')) Cal.open(); // calibration entrypoint opens this URL

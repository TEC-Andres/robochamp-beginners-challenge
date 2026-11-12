const Input = (() => {
// Input: touch widgets + physical gamepad (DualSense or any "standard" mapping pad) -> one pad state -> drive command.

const DEADZONE = 0.12;

// cro=✕ cir=○ squ=□ tri=△
const pad = { lx: 0, ly: 0, rx: 0, ry: 0, cro: 0, cir: 0, squ: 0, tri: 0, up: 0, down: 0, left: 0, right: 0 };

const dz = v => (Math.abs(v) < DEADZONE ? 0 : v);
const clamp = v => Math.max(-1, Math.min(1, v));

// □ / ○ step the speed down / up per press, 20%..100%
let scale = 0.7;
const speed = d => { scale = Math.max(0.2, Math.min(1, Math.round((scale + d) * 10) / 10)); };
const speedActions = { squ: () => speed(-0.1), cir: () => speed(0.1) };

// ---------- touch ----------
function bindStick(el, setX, setY) {
  const knob = el.querySelector('.knob');
  let id = null;
  function move(e) {
    const r = el.getBoundingClientRect(), R = r.width / 2;
    let dx = e.clientX - (r.left + R), dy = e.clientY - (r.top + R);
    const len = Math.hypot(dx, dy), max = R * 0.55;
    if (len > max) { dx *= max / len; dy *= max / len; }
    knob.style.transform = `translate(${dx}px, ${dy}px)`;
    setX(dx / max); setY(dy / max); // y: down is positive, like a gamepad
  }
  function end() { id = null; knob.style.transform = ''; setX(0); setY(0); el.classList.remove('on'); }
  el.addEventListener('pointerdown', e => { if (id !== null) return; id = e.pointerId; el.setPointerCapture(id); el.classList.add('on'); move(e); });
  el.addEventListener('pointermove', e => { if (e.pointerId === id) move(e); });
  el.addEventListener('pointerup', e => { if (e.pointerId === id) end(); });
  el.addEventListener('pointercancel', e => { if (e.pointerId === id) end(); });
}

function bindHold(el, key, onDown) {
  const set = v => { pad[key] = v; el.classList.toggle('on', !!v); };
  el.addEventListener('pointerdown', e => { el.setPointerCapture(e.pointerId); set(1); if (onDown) onDown(); });
  ['pointerup', 'pointercancel'].forEach(t => el.addEventListener(t, () => set(0)));
}

// actions: { tri } called on press, from touch or physical pad
function bindTouch(actions) {
  bindStick($('ls'), v => pad.lx = v, v => pad.ly = v);
  bindStick($('rs'), v => pad.rx = v, v => pad.ry = v);
  document.querySelectorAll('[data-btn]').forEach(el => bindHold(el, el.dataset.btn, actions[el.dataset.btn] || speedActions[el.dataset.btn]));
  document.querySelectorAll('[data-axis]').forEach(el => bindHold(el, el.dataset.axis));
  document.querySelectorAll('[data-dpad]').forEach(el => bindHold(el, el.dataset.dpad));
}

// ---------- physical gamepad ----------
// standard mapping: 0 ✕, 1 ○, 2 □, 3 △, 6 L2, 7 R2, 12-15 d-pad
const keyEls = [...document.querySelectorAll('[data-btn],[data-axis],[data-dpad]')];
let connected = false, prevTri = false, prevSqu = false, prevCir = false;

function showStick(el, x, y) {
  const max = el.getBoundingClientRect().width / 2 * 0.55;
  el.querySelector('.knob').style.transform = `translate(${x * max}px, ${y * max}px)`;
  el.classList.toggle('on', x !== 0 || y !== 0);
}

// actions: { tri }. Returns a label for the connected pad ('' if none).
function pollGamepad(actions) {
  const gp = [...(navigator.getGamepads?.() || [])].find(Boolean);
  if (!gp) { connected = false; return ''; }
  connected = true;
  const b = i => (gp.buttons[i] ? gp.buttons[i].value : 0);
  pad.lx = gp.axes[0] || 0; pad.ly = gp.axes[1] || 0; pad.rx = gp.axes[2] || 0; pad.ry = gp.axes[3] || 0;
  pad.cro = b(0); pad.cir = b(1); pad.squ = b(2); pad.tri = b(3);
  pad.up = b(12); pad.down = b(13); pad.left = b(14); pad.right = b(15);
  if (pad.tri && !prevTri) actions.tri();
  if (pad.squ && !prevSqu) speedActions.squ();
  if (pad.cir && !prevCir) speedActions.cir();
  prevTri = !!pad.tri; prevSqu = !!pad.squ; prevCir = !!pad.cir;
  // mirror the physical pad on screen
  keyEls.forEach(el => { const k = el.dataset.btn || el.dataset.axis || el.dataset.dpad; if (k in pad) el.classList.toggle('on', pad[k] > 0.1); });
  showStick($('ls'), dz(pad.lx), dz(pad.ly));
  showStick($('rs'), dz(pad.rx), dz(pad.ry));
  return /054c|dualsense|wireless controller/i.test(gp.id) ? 'DualSense connected' : 'Gamepad connected';
}

// ---------- mapping: pad -> [forward, turn] in -1..1 ----------
function command() {
  let y = -dz(pad.ly), w = dz(pad.lx) + dz(pad.rx);           // left stick moves + turns, right stick rotates; ✕ unused
  // d-pad: fixed full-speed moves (still scaled by □ / ○), overrides the sticks
  if (pad.up || pad.down) { y = pad.up - pad.down; w = 0; }
  if (pad.left || pad.right) { y = 0; w = pad.right - pad.left; }
  return [clamp(y * scale), clamp(w * scale)];
}

return { pad, bindTouch, pollGamepad, command };
})();

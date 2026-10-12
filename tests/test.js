const assert = require('node:assert/strict');
const fs = require('node:fs');
const vm = require('node:vm');
const path = require('node:path');
const root = path.resolve(__dirname, '..');
let passed = 0;
function test(name, fn) { fn(); passed++; console.log('PASS ' + name); }
const gaze = {};
vm.createContext(gaze);
vm.runInContext(fs.readFileSync(root + '/plasmoid/contents/ui/gaze.js', 'utf8'), gaze);
test('centered and invalid inputs are neutral', () => {
 for (const [x,y] of [[0,0],[NaN,1],[1,Infinity]]) {
  assert.equal(gaze.offset(x,y,12,11,160).x,0);
  assert.equal(gaze.offset(x,y,12,11,160).y,0);
 }
});
test('all quadrants stay inside ellipse', () => {
 for(let x=-4000;x<=4000;x+=61) for(let y=-3000;y<=3000;y+=59) {
  const p = gaze.offset(x,y,12,11,160);
  assert.ok((p.x/12)**2+(p.y/11)**2 <= 1.00000001);
  assert.ok(p.x*x>=0 && p.y*y>=0);
 }
});
test('logical negative monitor origins and fractional coordinates', () => {
 const center={x:-1279.5,y:940.25}, cursor={x:-2000.5,y:800.75};
 const p=gaze.offset(cursor.x-center.x,cursor.y-center.y,12,11,160);
 assert.ok(p.x<0 && p.y<0);
 const translated=gaze.offset((cursor.x+4000)-(center.x+4000),(cursor.y+100)-(center.y+100),12,11,160);
 assert.equal(p.x,translated.x); assert.equal(p.y,translated.y);
});
let onMove, tick, calls=[];
const context={workspace:{cursorPos:{x:10,y:20},cursorPosChanged:{connect(f){onMove=f;}}},
 QTimer:function(){this.timeout={connect(f){tick=f;}};this.start=function(){};},
 callDBus(...args){calls.push(args);}};
vm.createContext(context);
vm.runInContext(fs.readFileSync(root+'/kwin/contents/code/main.js','utf8'),context);
test('initial sample and correct stable string transport',()=>{
 tick(); assert.equal(calls.length,1); assert.equal(calls[0][0],'org.loop.Eyes');
 assert.deepEqual(JSON.parse(calls[0][4]),{x:10,y:20});
});
test('high-rate movement coalesces and trailing sample survives',()=>{
 calls=[]; for(let i=0;i<1000;i++){context.workspace.cursorPos={x:i,y:-i};onMove();}
 assert.equal(calls.length,0); tick(); assert.equal(calls.length,1);
 assert.deepEqual(JSON.parse(calls[0][4]),{x:999,y:-999});
 tick(); assert.equal(calls.length,1);
});
test('stationary heartbeat and missing receiver recovery need no callback',()=>{
 calls=[]; for(let i=0;i<60;i++) tick();
 assert.equal(calls.length,2); assert.equal(calls[0].length,5);
});
test('package metadata and avatar paths exist',()=>{
 for(const [folder,id] of [['plasmoid','org.loop.eyes'],['kwin','org.loop.eyes.cursor']]) {
 const p=JSON.parse(fs.readFileSync(root+'/'+folder+'/metadata.json'));
 assert.equal(p.KPlugin.Id,id);
 }
 assert.ok(fs.statSync(root+'/plasmoid/contents/images/loop-avatar.png').size>0);
});
console.log(`${passed} tests passed`);

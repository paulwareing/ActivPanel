const logDiv = document.getElementById('log');

function logEvent(event) {
  const { type, clientX, clientY, pointerType, width, height, pressure } = event;
  const pointerInfo = pointerType ? ` [pointerType: ${pointerType}]` : '';
  const sizeInfo = width && height ? ` [size: ${Math.round(width)}x${Math.round(height)}]` : '';
  const pressureInfo = pressure !== undefined ? ` [pressure: ${pressure.toFixed(2)}]` : '';
  const logEntry = `${new Date().toLocaleTimeString()} - ${type} at (${Math.round(clientX)}, ${Math.round(clientY)})${pointerInfo}${sizeInfo}${pressureInfo}\n`;
  logDiv.textContent += logEntry;
  logDiv.scrollTop = logDiv.scrollHeight;
}

// Only log pointer events
const eventsToLog = ['pointerdown', 'pointermove', 'pointerup', 'pointercancel'];

// Attach event listeners for pointer events
eventsToLog.forEach(eventType => {
  document.addEventListener(eventType, event => {
    if (event.touches && event.touches.length > 0) {
      const touch = event.touches[0];
      logEvent({ type: event.type, clientX: touch.clientX, clientY: touch.clientY, width: touch.radiusX * 2, height: touch.radiusY * 2, pressure: touch.force });
    } else if (event.changedTouches && event.changedTouches.length > 0) {
      const touch = event.changedTouches[0];
      logEvent({ type: event.type, clientX: touch.clientX, clientY: touch.clientY, width: touch.radiusX * 2, height: touch.radiusY * 2, pressure: touch.force });
    } else {
      logEvent(event);
    }
  });
});
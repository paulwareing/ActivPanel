const logDiv = document.getElementById('log');

function logEvent(event) {
  const { type, clientX, clientY, pointerType, width, height, pressure, pointerId } = event;
  const pointerInfo = pointerType ? ` [pointerType: ${pointerType}]` : '';
  const pointerIdInfo = pointerId !== undefined ? ` [pointerId: ${pointerId}]` : '';
  const positionInfo = ` at (${Math.round(clientX)}, ${Math.round(clientY)})`;
  const sizeInfo = width && height ? ` [size: ${Math.round(width)}x${Math.round(height)}]` : '';
  const pressureInfo = pressure !== undefined ? ` [pressure: ${pressure.toFixed(2)}]` : '';
  const logEntry = `${new Date().toLocaleTimeString()} - ${type}${pointerInfo}${pointerIdInfo}${positionInfo}${sizeInfo}${pressureInfo}\n`;
  logDiv.textContent += logEntry;
  logDiv.scrollTop = logDiv.scrollHeight;
}

// Only log pointer events
const eventsToLog = ['pointerdown', 'pointermove', 'pointerup', 'pointercancel'];

// Attach event listeners for pointer events
eventsToLog.forEach(eventType => {
  document.addEventListener(eventType, event => {
    if (event.touches && event.touches.length > 0) {
      Array.from(event.touches).forEach(touch => {
        console.debug(`Touch identifier: ${touch.identifier}, Pointer ID: ${event.pointerId}`); // Debug log for identifier and pointerId
        logEvent({
          type: event.type,
          clientX: touch.clientX,
          clientY: touch.clientY,
          width: touch.radiusX * 2,
          height: touch.radiusY * 2,
          pressure: touch.force,
          pointerId: event.pointerId
        });
      });
    } else {
      logEvent(event);
    }
  });
});
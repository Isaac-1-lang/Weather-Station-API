# Troubleshooting: "Waiting for data..." Issue

If you're seeing "Waiting for data..." and no data is appearing, follow these steps:

## 🔍 Step 1: Check Browser Console

1. **Open Browser Developer Tools**:
   - Press `F12` or `Ctrl+Shift+I` (Windows/Linux)
   - Or `Cmd+Option+I` (Mac)
   - Go to the **Console** tab

2. **Look for these messages**:
   - ✅ `Connected to MQTT broker via WebSocket` - Connection successful
   - ✅ `Subscribed to weather/station/data` - Subscription successful
   - 📨 `MQTT Message received!` - Messages are arriving
   - ❌ Any error messages in red

## 🔍 Step 2: Check Connection Status

Look at the status indicator at the top of the dashboard:
- **Green "✓ Connected"** = Dashboard is connected to MQTT broker
- **Red "✗ Connection error"** = WebSocket connection failed
- **Yellow "⟳ Reconnecting"** = Trying to reconnect

### If you see "Connection error":

1. **Check WebSocket URL** (line 423 in dashboard.html):
   ```javascript
   const MQTT_BROKER_WS = 'ws://157.173.101.159:9001';
   ```
   - Verify this IP/port is correct for your broker
   - Make sure port 9001 is open (not blocked by firewall)

2. **Try alternative WebSocket URLs**:
   - `ws://broker.benax.rw:9001` (if using hostname)
   - `wss://157.173.101.159:9001` (if using secure WebSocket)

3. **Check if MQTT broker has WebSocket support enabled**:
   - WebSocket support must be configured in Mosquitto
   - Default WebSocket port is usually 9001

## 🔍 Step 3: Check ESP8266 Serial Output

1. **Open Arduino Serial Monitor** (115200 baud)
2. **Look for these messages**:
   ```
   WiFi connected
   IP Address: 192.168.x.x
   Connecting to MQTT...
   connected
   Subscribed to: weather/station/led/control
   Temperature: XX.XX °C, Humidity: XX.XX %
   ✓ Published to weather/station/data: {"temperature":XX.XX,"humidity":XX.XX}
   ```

### If ESP8266 shows "Failed to publish":
- Check MQTT broker IP (line 10 in index.ino)
- Verify ESP8266 is connected to WiFi
- Check if MQTT broker is running and accessible

### If ESP8266 shows "Failed to read from DHT sensor":
- Check DHT sensor wiring (GPIO5)
- Verify sensor is working
- Check power supply

## 🔍 Step 4: Verify MQTT Topics Match

**Dashboard subscribes to:**
- `weather/station/data` (sensor data)
- `weather/station/led/status` (LED status)

**ESP8266 publishes to:**
- `weather/station/data` (sensor data) - Line 12 in index.ino
- `weather/station/led/status` (LED status) - Line 14 in index.ino

**ESP8266 subscribes to:**
- `weather/station/led/control` (LED commands) - Line 13 in index.ino

**Make sure these topics match exactly!**

## 🔍 Step 5: Test MQTT Connection Manually

### Option A: Use MQTT Client Tool
1. Install an MQTT client (like MQTT Explorer, MQTT.fx, or mosquitto_pub)
2. Connect to your broker: `157.173.101.159:1883`
3. Subscribe to: `weather/station/data`
4. Check if ESP8266 is publishing messages

### Option B: Use Browser Console
1. Open browser console (F12)
2. Type: `client.publish('weather/station/data', '{"temperature":25.5,"humidity":60.0}')`
3. Check if dashboard receives it

## 🔍 Step 6: Common Issues & Solutions

### Issue: "Connection error" in dashboard
**Solutions:**
- Verify WebSocket port 9001 is open
- Check if broker supports WebSockets
- Try different WebSocket URL format
- Check firewall settings

### Issue: Dashboard connects but no data
**Solutions:**
- Check ESP8266 Serial Monitor - is it publishing?
- Verify topics match exactly
- Check if ESP8266 is connected to MQTT
- Look for error messages in browser console

### Issue: ESP8266 can't connect to MQTT
**Solutions:**
- Verify MQTT broker IP (line 10 in index.ino)
- Check WiFi connection
- Verify broker is running
- Check broker port (1883 for MQTT, 9001 for WebSocket)

### Issue: "Failed to read from DHT sensor"
**Solutions:**
- Check sensor wiring (GPIO5)
- Verify sensor power (3.3V or 5V)
- Check sensor type (DHT11 vs DHT22)
- Try different GPIO pin

## 🔍 Step 7: Verify WebSocket Configuration

Your MQTT broker (Mosquitto) needs WebSocket support. Check `mosquitto.conf`:

```conf
listener 1883
listener 9001
protocol websockets
```

If WebSocket is not configured, the dashboard won't connect!

## 📊 Debug Checklist

- [ ] Browser console shows "Connected to MQTT broker"
- [ ] Browser console shows "Subscribed to weather/station/data"
- [ ] ESP8266 Serial Monitor shows "connected"
- [ ] ESP8266 Serial Monitor shows "✓ Published to weather/station/data"
- [ ] Topics match exactly between ESP8266 and dashboard
- [ ] WebSocket port 9001 is accessible
- [ ] MQTT broker has WebSocket support enabled
- [ ] No firewall blocking connections

## 🆘 Still Not Working?

1. **Share browser console output** (copy all messages)
2. **Share ESP8266 Serial Monitor output** (copy all messages)
3. **Check MQTT broker logs** (if you have access)
4. **Verify network connectivity** between:
   - ESP8266 → MQTT broker (port 1883)
   - Browser → MQTT broker (port 9001)

## 💡 Quick Test

To test if the dashboard is working:
1. Open browser console
2. Manually publish a test message:
   ```javascript
   client.publish('weather/station/data', '{"temperature":25.5,"humidity":60.0}')
   ```
3. If dashboard updates, the issue is with ESP8266 publishing
4. If dashboard doesn't update, the issue is with message handling

#!/usr/bin/env python3
import json
import websocket
import time

# URL zu Deinem ESP32-WebSocket (anpassen!)
ESP32_WS_URL = "ws://192.168.178.49/ws"

# Alle Testfälle, komplett:
# Für jedes Event (system, log, serial) jeweils alle implementierten keys/commands
TEST_CASES = [
    {"name": "log:debug status",		"payload":{"type":"log","command":"debug","key":"status","value":""}, 				"expected": {"event":"log","action":"debug","status":"success",	"details":{"activate": bool,"detail": str}}},
	{"name": "log:files list",			"payload": {"type":"log","command":"files","key":"list","value":""},				"expected": {"event":"log","action":"files",	"status":"success",	"details": {"list": list}}},
    {"name": "log:unknown cmd",			"payload": {"type":"log","command":"foo","key":"bar","value":""},					"expected": {"event":"log","action":"response", "status": "error", 	"details": {"errorDetail": str}}},
]

# Hilfsfunktion: flusht alle bisherigen Nachrichten (z.B. Initial-Status)
def flush(ws):
    """Leert alle anstehenden Nachrichten aus dem Socket."""
    ws.settimeout(0.2)
    try:
        while True:
            ws.recv()
    except:
        pass
    ws.settimeout(5)

def compare(expected, actual):
    """
    Vergleicht die erwarteten Felder (inkl. Typen für 'details')
    mit der tatsächlichen Antwort.
    """
    # 1) Primitive Felder
    for k, v in expected.items():
        if k == "details":
            continue
        if actual.get(k) != v:
            return False, f"Field '{k}': got {actual.get(k)!r}, expected {v!r}"

    # 2) Details-Objekt
    if "details" in expected:
        exp_det = expected["details"]
        act_det = actual.get("details")
        if not isinstance(act_det, dict):
            return False, f"'details' is {type(act_det).__name__}, expected dict"
        # Für jedes erwartete Feld prüfen wir nur den Typ
        for field, typ in exp_det.items():
            if field not in act_det:
                return False, f"Missing 'details' field: '{field}'"
            if typ is not None and not isinstance(act_det[field], typ):
                return False, f"details['{field}'] is {type(act_det[field]).__name__}, expected {typ.__name__}"

    return True, ""

def run_test(case):
    print(f"\n--- Test: {case['name']} ---")
    ws = websocket.create_connection(ESP32_WS_URL, timeout=5)
    try:
        flush(ws)
        payload = json.dumps(case["payload"])
        print("Sende:     ", payload)
        ws.send(payload)

        found = False
        start = time.time()
        while time.time() - start < 5:
            resp = ws.recv()
            data = json.loads(resp)
            exp = case["expected"]
            if data.get("event") == exp["event"] and data.get("action") == exp["action"]:
                ok, msg = compare(exp, data)
                print("Empfange:  ", resp)
                print("Erwartet:  ", exp)
                print("Ergebnis:  ", "OK" if ok else f"FAIL ({msg})")
                found = True
                break

        if not found:
            print("Ergebnis:  TIMEOUT – keine passende Antwort erhalten.")
    except Exception as e:
        print("FEHLER:", e)
    finally:
        ws.close()

if __name__ == "__main__":
    print("Starte WebSocket-Tests gegen", ESP32_WS_URL)
    for tc in TEST_CASES:
        run_test(tc)

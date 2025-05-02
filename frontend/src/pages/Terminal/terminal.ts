import { defineComponent, computed, ref, watch, nextTick, provide, onMounted, onUnmounted } from "vue";

import { SocketService } from "@/_service/socket";
import { useSystemStore } from "@/store/system/index.store";

// Components
import Widget from "@/components/Widget/widget.vue";

export default defineComponent({
	name: "Terminal",
	components: { Widget },
	setup() {
		const terminalOutput = ref("");
		const terminalTextarea = ref<HTMLTextAreaElement | null>(null);
		const command = ref<HTMLInputElement | null>(null);

		// const isFullscreen = ref(false);
		const isFullscreen = ref(false);
		const showModal = ref(false);

		const selectedBaudRate = ref(1200);
		const baudRates = ref([1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200]);

		const systemStore = useSystemStore();

		const scrollToBottom = () => {
			const ta = terminalTextarea.value;
			if (ta) ta.scrollTop = ta.scrollHeight;
		};

		const toggleConnection = () => {
			if (!systemStore.serial.connected) {
				// Öffne das Modal NUR beim Verbinden
				showModal.value = true;
			} else {
				// Trenne die Verbindung direkt
				systemStore.serial.connected = false;
			}
		};

		const closeBaudrateModal = () => {
			showModal.value = false;
		};

		const confirmConnection = async () => {
			try {
				console.log("Baudrate setzen:", selectedBaudRate.value);
				// Promise, das auf die Status-Antwort wartet
				const statusPromise = new Promise<void>((resolve, reject) => {
					const statusHandler = (data: { status: string; details: { available: boolean; baudRate: number } }) => {
						console.log("Serial-Status erhalten:", data);
						if (data.status === "success" && data.details?.available) {
							console.log("Serial verbunden mit Baudrate:", data.details.baudRate);
							systemStore.serial.connected = true;
							isFullscreen.value = true; // Vollbild aktivieren
							resolve();
						} else {
							reject(new Error("Serial-Status nicht verfügbar"));
						}
						SocketService.removeListener("serial", "status", statusHandler);
					};
					SocketService.onMessage("serial", "status", statusHandler);
				});
				// Sende den Befehl, um Baudrate zu setzen
				await SocketService.sendMessage({
					type: "serial",
					command: "setBaud",
					key: "",
					value: selectedBaudRate.value.toString().trim(),
				});
				// Warte auf den erfolgreichen Status
				await statusPromise;
				// Verbindung erfolgreich
				// systemStore.serial.connected = true;
			} catch (err) {
				console.warn("Verbindung konnte nicht hergestellt werden:", err);
				systemStore.serial.connected = false;
			} finally {
				showModal.value = false;
			}
		};

		const toggleFullscreen = () => {
			isFullscreen.value = !isFullscreen.value;
		};

		const sendData = async () => {
			if (!systemStore.serial.connected) return;
			const raw = command.value?.value ?? "";
			const data = raw.trim();

			try {
				console.log("Serial senden:", data);
				if (data) {
					await SocketService.sendMessage({
						type: "serial",
						command: "send",
						key: "",
						value: data,
					});
					command.value!.value = ""; // Input zurücksetzen
				} else {
					console.warn("Leere Eingabe");
				}
				// nach dem DOM-Update scrollen
			} catch (error) {
				const errorMessage = error instanceof Error ? error.message : "Unbekannter Fehler";
				terminalOutput.value += `\nError: ${errorMessage}\n`;
			}
		};

		const handleSerialData = (data: { status: string; details: string }) => {
			if (data.status === "data") {
				terminalOutput.value += data.details || "";
			} else {
				terminalOutput.value += `Error: ${data.details || "Unbekannter Fehler."}`;
			}
			// nach dem DOM-Update scrollen
			nextTick(scrollToBottom);
		};

		watch(isFullscreen, (newVal) => {
			const terminalEl = document.getElementById("terminal");
			if (!terminalEl) {
				console.warn("#terminal nicht gefunden.");
				return;
			}
			if (newVal) {
				terminalEl.classList.add("fullscreen");
			} else {
				terminalEl.classList.remove("fullscreen");
			}
		});
		onMounted(() => {
			SocketService.onMessage("serial", "incoming", handleSerialData);
			systemStore.serial.connected = false; // Initialzustand
		});

		onUnmounted(() => {});

		return {
			systemStore,
			showModal,
			selectedBaudRate,
			baudRates,
			toggleConnection,
			closeBaudrateModal,
			confirmConnection,
			toggleFullscreen,
			terminalOutput,
			terminalTextarea,
			command,
			sendData,
		};
	},
});

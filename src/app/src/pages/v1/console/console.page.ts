import { defineComponent, ref, onMounted, onUnmounted, nextTick } from "vue";
import { SerialService } from "@/services/serial/serial.service";

// Mixins
import LayoutMixin from "@/mixins/layout.mixin";

// Services

// Layouts

// Componetns

/**
 * App - The root Vue component of the application.
 *
 * This component is responsible for initializing the application's state
 * and setting up the main layout.
 */
export default defineComponent({
	name: "ConsolePage",
	data() {
		return [];
	},
	mixins: [LayoutMixin],
	setup: () => {
		const terminalOutput = ref<string>("Willkommen im Serial Terminal!\n");

		const userInput = ref<string>("");
		const isConnected = ref<boolean>(false);
		const selectedBaudRate = ref<number>(1200);
		const baudRates = ref<number[]>([
			1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200,
		]);

		// Berechne die Höhe des Containers
		const fullHeight = ref(`${window.innerHeight - 80}px`);

		// Event handler for SerialService
		const handleSerialData = (status: string, details?: string) => {
			if (status === "success") {
				terminalOutput.value +=
					details || "Empfangene Daten: Keine Details verfügbar.";
			} else {
				terminalOutput.value += `Error: ${
					details || "Unbekannter Fehler."
				}`;
			}
		};

		const handleReceiveData = (message: string) => {
			if (!isConnected.value) {
				isConnected.value = true;
				terminalOutput.value += `\nVerbindung erkannt.\n`;
			}
			terminalOutput.value += `${message}`;

			// Automatisch scrollen
			nextTick(() => {
				const terminal = document.getElementById("terminalOutput");
				if (terminal) {
					terminal.scrollTop = terminal.scrollHeight;
				}
			});
		};

		// Toggle connection
		const toggleConnection = async () => {
			if (isConnected.value) {
				try {
					await SerialService.disconnect();
					isConnected.value = false;
					terminalOutput.value += "\nVerbindung getrennt.\n";
				} catch (error) {
					const errorMessage =
						error instanceof Error
							? error.message
							: "Unbekannter Fehler";
					terminalOutput.value += `\nError: ${errorMessage}\n`;
				}
			} else {
				try {
					await SerialService.setBaudRate(selectedBaudRate.value);
					isConnected.value = true;
					terminalOutput.value += `\nVerbunden mit ${selectedBaudRate.value} Baud.\n`;
				} catch (error) {
					const errorMessage =
						error instanceof Error
							? error.message
							: "Unbekannter Fehler";
					terminalOutput.value += `\nError: ${errorMessage}\n`;
				}
			}
		};

		// Send user input
		const sendData = async () => {
			if (!isConnected.value || userInput.value.trim() === "") return;

			terminalOutput.value += `\n$ ${userInput.value}\n`;
			try {
				await SerialService.sendMessage(userInput.value);
			} catch (error) {
				const errorMessage =
					error instanceof Error
						? error.message
						: "Unbekannter Fehler";
				terminalOutput.value += `\nError: ${errorMessage}\n`;
			}

			userInput.value = ""; // Clear input
		};

		// Lifecycle hooks
		onMounted(() => {
			const updateHeight = () => {
				fullHeight.value = `${window.innerHeight - 300}px`;
			};

			window.addEventListener("resize", updateHeight);

			// Initiale Höhe setzen
			updateHeight();

			SerialService.onMessage(handleSerialData);
			SerialService.onReceive((message: string) => {
				handleReceiveData(message);
				if (!isConnected.value) {
					isConnected.value = true;
					terminalOutput.value += `\nVerbindung erkannt beim Start.\n`;
				}
			});
		});

		onUnmounted(() => {
			SerialService.removeListener(handleSerialData);
			SerialService.removeReceiveListener(handleReceiveData); // Listener entfernen
			window.removeEventListener("resize", () => {
				fullHeight.value = `${window.innerHeight - 300}px`;
			});
		});

		return {
			terminalOutput,
			userInput,
			isConnected,
			selectedBaudRate,
			baudRates,
			toggleConnection,
			sendData,
			fullHeight,
		};
	},
	methods: {
		getLayoutClass() {
			return "grid-cols-1subgrid";
		},
	},

	afterMount() {},
	mounted() {},
	beforeUnmount() {},
});

import { ref, onMounted, defineComponent } from "vue";

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
		return {
			terminalLines: [
				"Willkommen im Terminal! Geben Sie einen Befehl ein:",
			],
			userInput: "",
			isConnected: false,
			selectedBaudRate: 9600,
			baudRates: [9600, 14400, 19200, 38400, 57600, 115200], // Baudrate-Optionen
			dummyDataInterval: null as number | null, // Speichert die Intervall-ID
			sectionHeight: "auto", // Dynamische Höhe für die Section
		};
	},
	mixins: [LayoutMixin],
	methods: {
		getLayoutClass() {
			return "grid-cols-1subgrid";
		},
		handleCommand() {
			if (this.userInput.trim() === "") return;
			this.terminalLines.push(`$ ${this.userInput}`);
			this.terminalLines.push(`Ausgabe für "${this.userInput}": ...`);
			this.userInput = "";
			this.scrollToBottom();
		},
		startDummyData() {
			this.dummyDataInterval = setInterval(() => {
				this.terminalLines.push(
					`Dummy-Daten: ${new Date().toISOString()}`
				);
				this.scrollToBottom();
			}, 250) as unknown as number; // Typumwandlung von Timeout zu number
		},
		scrollToBottom() {
			this.$nextTick(() => {
				setTimeout(() => {
					const terminalOutput = this.$refs
						.terminalOutput as HTMLDivElement;
					if (terminalOutput) {
						terminalOutput.scrollTop = terminalOutput.scrollHeight;
					} else {
						console.error("terminalOutput ist nicht verfügbar");
					}
				}, 0); // Verzögerung, um sicherzustellen, dass der DOM-Update-Prozess abgeschlossen ist
			});
		},
		stopDummyData() {
			if (this.dummyDataInterval !== null) {
				clearInterval(this.dummyDataInterval);
				this.dummyDataInterval = null;
			}
		},
		toggleConnection() {
			if (this.isConnected) {
				this.terminalLines.push("Verbindung getrennt.");
				this.stopDummyData();
			} else {
				this.terminalLines.push(
					`Verbunden mit ${this.selectedBaudRate} Baud.`
				);
				this.startDummyData();
			}
			this.isConnected = !this.isConnected;
		},
		updateSectionHeight() {
			const headerHeight =
				document.querySelector("header")?.offsetHeight || 0;
			const footerHeight =
				document.querySelector("footer")?.offsetHeight || 0;
			const windowHeight = window.innerHeight;

			this.sectionHeight = `${
				windowHeight - headerHeight - footerHeight - 12
			}px`;
		},
	},

	afterMount() {
		const terminalOutput = this.$refs.terminalOutput as HTMLDivElement;
		if (!terminalOutput) {
			console.error("terminalOutput ist nicht verfügbar");
		}
	},
	mounted() {
		this.updateSectionHeight();
		window.addEventListener("resize", this.updateSectionHeight);
	},
	beforeUnmount() {
		this.stopDummyData();
		window.removeEventListener("resize", this.updateSectionHeight);
	},
});

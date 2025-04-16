import { defineComponent, ref, onMounted, onUnmounted } from "vue";

// Types

// Components

// Templates

export default defineComponent({
	name: "Offline",
	components: {},
	setup() {
		// Ref für den Online-/Offline-Status (initial anhand von navigator.onLine)
		const onlineStatus = ref(navigator.onLine);

		// Funktion, um den Status zu aktualisieren
		const updateOnlineStatus = () => {
			onlineStatus.value = navigator.onLine;
		};

		// Event-Listener beim Mounten der Komponente registrieren
		onMounted(() => {
			window.addEventListener("online", updateOnlineStatus);
			window.addEventListener("offline", updateOnlineStatus);
		});

		// Event-Listener beim Unmounten wieder entfernen, um Memory-Leaks zu vermeiden
		onUnmounted(() => {
			window.removeEventListener("online", updateOnlineStatus);
			window.removeEventListener("offline", updateOnlineStatus);
		});

		return { onlineStatus };
	},
});

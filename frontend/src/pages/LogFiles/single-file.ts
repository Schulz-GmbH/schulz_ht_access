import { defineComponent, ref, onMounted } from "vue";
import { useRoute, useRouter } from "vue-router";

import { getLogByFilename, deleteLogByFilename } from "@/_utils/log/IndexedDBService";
// Components

export default defineComponent({
	name: "SingleFile",
	components: {},
	setup() {
		const route = useRoute();
		const router = useRouter();
		const filename = route.params.filename as string;

		const content = ref<string>("");

		// lädt den Log-Eintrag
		onMounted(async () => {
			const rec = await getLogByFilename(filename);
			content.value = rec?.log ?? "Kein Log gefunden.";
		});

		// Umbenennen
		async function rename() {
			const newName = window.prompt("Neuer Dateiname:", filename);
			if (!newName || newName.trim() === "" || newName === filename) return;
			const rec = await getLogByFilename(filename);
			if (!rec) return;
			// alten Eintrag löschen und unter neuem Namen speichern
			await deleteLogByFilename(filename);
			// await putLogRecord({
			// 	filename: newName,
			// 	log: rec.log,
			// 	timestamp: rec.timestamp,
			// });
			router.replace({ name: "LogFile", params: { filename: newName } });
		}

		// Löschen
		async function remove() {
			if (!window.confirm(`"${filename}" wirklich löschen?`)) return;
			await deleteLogByFilename(filename);
			router.push({ name: "LogFiles" });
		}

		// Versenden (z.B. über Web Share API oder Download)
		async function send() {
			if (navigator.share) {
				try {
					await navigator.share({
						title: filename,
						text: content.value,
					});
				} catch {}
			} else {
				// Fallback: als .txt herunterladen
				const blob = new Blob([content.value], { type: "text/plain" });
				const url = URL.createObjectURL(blob);
				const a = document.createElement("a");
				a.href = url;
				a.download = filename;
				document.body.appendChild(a);
				a.click();
				document.body.removeChild(a);
				URL.revokeObjectURL(url);
			}
		}

		return {
			filename,
			content,
			rename,
			remove,
			send,
		};
	},
});

import { defineComponent, ref, onMounted } from "vue";
import { useRoute, useRouter } from "vue-router";
import { getLogByFilename, deleteLogByFilename } from "@/_utils/log/IndexedDBService";

export default defineComponent({
	name: "SingleFile",
	setup() {
		const route = useRoute();
		const router = useRouter();
		const filename = route.params.filename as string;
		const content = ref("");

		onMounted(async () => {
			const rec = await getLogByFilename(filename);
			content.value = rec?.log ?? "Kein Log gefunden.";
		});

		async function rename() {
			const newName = window.prompt("Neuer Dateiname:", filename);
			if (!newName || newName.trim() === "" || newName === filename) return;
			await deleteLogByFilename(filename);
			router.replace({ name: "LogFile", params: { filename: newName } });
		}

		async function remove() {
			if (!window.confirm(`"${filename}" wirklich löschen?`)) return;
			await deleteLogByFilename(filename);
			router.push({ name: "LogFiles" });
		}

		async function send() {
			if (navigator.share) {
				await navigator.share({ title: filename, text: content.value });
			} else {
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

		return { filename, content, rename, remove, send };
	},
});

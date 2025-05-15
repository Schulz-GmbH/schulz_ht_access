import { defineComponent, ref, onMounted } from "vue";
import { useRouter } from "vue-router";

import { useSystemStore } from "@/store/system/index.store";

// Components
import Table from "@/components/Table/table.vue";

import type { TableColumn } from "@/components/Table/_types/column";
import type { TableMeta } from "@/components/Table/_types/meta";
import type { TableParams } from "@/components/Table/_types/params";

import { getAllLogRecords } from "@/_utils/log/IndexedDBService";

interface LogRecord {
	filename: string;
	timestamp: string;
	log: string;
}

export default defineComponent({
	name: "LogFiles",
	components: { Table },
	setup() {
		const router = useRouter();
		const rawLogs = ref<LogRecord[]>([]);
		const logs = ref<any[]>([]);
		const params = ref<TableParams>({ size: 10, page: 1 });
		const meta = ref<TableMeta>({ totalElements: 0, totalPages: 0, from: 0, to: 0 });

		const columns: TableColumn[] = [
			{ key: "filename", label: "Filename", sortable: true },
			{ key: "date", label: "Datum", sortable: true },
			{ key: "view", label: "", sortable: false, slot: "view" },
			{ key: "action", label: "", sortable: false, slot: "action" },
		];

		async function loadLogs() {
			const all = await getAllLogRecords();
			// sortiere absteigend nach timestamp
			all.sort((a, b) => (a.timestamp < b.timestamp ? 1 : -1));
			rawLogs.value = all;

			// baue Tabelle
			const start = (params.value.page - 1) * params.value.size;
			const slice = rawLogs.value.slice(start, start + params.value.size);
			logs.value = slice.map((r) => ({
				filename: r.filename,
				date: r.timestamp.split("T")[0],
				// view: `<button class="btn btn-sm">Ansehen</button>`,
				// action: `<button class="btn btn-danger btn-sm">Löschen</button>`,
			}));

			meta.value = {
				totalElements: rawLogs.value.length,
				totalPages: Math.ceil(rawLogs.value.length / params.value.size),
				from: start + 1,
				to: start + slice.length,
			};
		}

		// Neu laden bei Seiten- oder Größenwechsel
		function onParamsChange(newParams: TableParams) {
			params.value = newParams;
			loadLogs();
		}

		function viewLog(filename: string) {
			// zu /log/<filename> routen
			router.push({ path: `/log/${filename}` });
		}

		async function deleteLog(filename: string) {
			if (confirm(`Wirklich "${filename}" löschen?`)) {
				// await deleteLogByFilename(filename);
				loadLogs();
			}
		}

		onMounted(() => {
			loadLogs();
		});

		return {
			columns,
			logs,
			meta,
			params,
			onParamsChange,
			viewLog,
			deleteLog,
		};
	},
});

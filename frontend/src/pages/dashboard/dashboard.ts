import { defineComponent, onMounted, computed, ref } from "vue";

import type { TableColumn } from "@/components/Table/_types/column";
import type { TableMeta } from "@/components/Table/_types/meta";
import type { TableParams } from "@/components/Table/_types/params";

// Stelle sicher, dass der Pfad zu deiner Router-Instanz korrekt ist

// Components
import Widget from "@/components/Widget/widget.vue";
import Table from "@/components/Table/table.vue";
import { useSystemStore } from "@/store/system/index.store";

export default defineComponent({
	name: "Dashboard",
	components: { Widget, Table },
	setup() {
		const systemStore = useSystemStore();

		const columns: TableColumn[] = [
			{ key: "filename", label: "Log" },
			{ key: "date", label: "Datum" },
			{ key: "view", label: "View", sortable: false },
			{ key: "action", label: "Action", sortable: false },
		];

		let meta: TableMeta;
		let params: TableParams = {
			size: 5,
			page: 1,
			recommendation: ["BUY", "SELL", "NONE"],
		};

		const logs = ref([
			{ id: 1, filename: "typ-e-2024-04-03.log", date: "2024-04-03", view: "View", action: "Action" },
			{ id: 2, filename: "typ-e-2024-04-02.log", date: "2024-04-02", view: "View", action: "Action" },
			{ id: 3, filename: "typ-e-2024-04-01.log", date: "2024-04-01", view: "View", action: "Action" },
			{ id: 4, filename: "typ-e-2024-03-31.log", date: "2024-03-31", view: "View", action: "Action" },
			{ id: 5, filename: "typ-e-2024-03-30.log", date: "2024-03-30", view: "View", action: "Action" },
			{ id: 6, filename: "typ-e-2024-03-29.log", date: "2024-03-29", view: "View", action: "Action" },
			{ id: 7, filename: "typ-e-2024-03-28.log", date: "2024-03-28", view: "View", action: "Action" },
			{ id: 8, filename: "typ-e-2024-03-27.log", date: "2024-03-27", view: "View", action: "Action" },
			{ id: 9, filename: "typ-e-2024-03-26.log", date: "2024-03-26", view: "View", action: "Action" },
			{ id: 10, filename: "typ-e-2024-03-25.log", date: "2024-03-25", view: "View", action: "Action" },
			{ id: 11, filename: "typ-e-2024-03-24.log", date: "2024-03-24", view: "View", action: "Action" },
			{ id: 12, filename: "typ-e-2024-03-23.log", date: "2024-03-23", view: "View", action: "Action" },
			{ id: 13, filename: "typ-e-2024-03-22.log", date: "2024-03-22", view: "View", action: "Action" },
			{ id: 14, filename: "typ-e-2024-03-21.log", date: "2024-03-21", view: "View", action: "Action" },
			{ id: 15, filename: "typ-e-2024-03-20.log", date: "2024-03-20", view: "View", action: "Action" },
			{ id: 16, filename: "typ-e-2024-03-19.log", date: "2024-03-19", view: "View", action: "Action" },
			{ id: 17, filename: "typ-e-2024-03-18.log", date: "2024-03-18", view: "View", action: "Action" },
			{ id: 18, filename: "typ-e-2024-03-17.log", date: "2024-03-17", view: "View", action: "Action" },
			{ id: 19, filename: "typ-e-2024-03-16.log", date: "2024-03-16", view: "View", action: "Action" },
			{ id: 20, filename: "typ-e-2024-03-15.log", date: "2024-03-15", view: "View", action: "Action" },
			{ id: 21, filename: "typ-e-2024-03-14.log", date: "2024-03-14", view: "View", action: "Action" },
			{ id: 22, filename: "typ-e-2024-03-13.log", date: "2024-03-13", view: "View", action: "Action" },
			{ id: 23, filename: "typ-e-2024-03-12.log", date: "2024-03-12", view: "View", action: "Action" },
			{ id: 24, filename: "typ-e-2024-03-11.log", date: "2024-03-11", view: "View", action: "Action" },
			{ id: 25, filename: "typ-e-2024-03-10.log", date: "2024-03-10", view: "View", action: "Action" },
			{ id: 26, filename: "typ-e-2024-03-09.log", date: "2024-03-09", view: "View", action: "Action" },
			{ id: 27, filename: "typ-e-2024-03-08.log", date: "2024-03-08", view: "View", action: "Action" },
			{ id: 28, filename: "typ-e-2024-03-07.log", date: "2024-03-07", view: "View", action: "Action" },
			{ id: 29, filename: "typ-e-2024-03-06.log", date: "2024-03-06", view: "View", action: "Action" },
			{ id: 30, filename: "typ-e-2024-03-05.log", date: "2024-03-05", view: "View", action: "Action" },
			{ id: 31, filename: "typ-e-2024-03-04.log", date: "2024-03-04", view: "View", action: "Action" },
			{ id: 32, filename: "typ-e-2024-03-03.log", date: "2024-03-03", view: "View", action: "Action" },
			{ id: 33, filename: "typ-e-2024-03-02.log", date: "2024-03-02", view: "View", action: "Action" },
			{ id: 34, filename: "typ-e-2024-03-01.log", date: "2024-03-01", view: "View", action: "Action" },
			{ id: 35, filename: "typ-e-2024-02-29.log", date: "2024-02-29", view: "View", action: "Action" },
			{ id: 36, filename: "typ-e-2024-02-28.log", date: "2024-02-28", view: "View", action: "Action" },
			{ id: 37, filename: "typ-e-2024-02-27.log", date: "2024-02-27", view: "View", action: "Action" },
			{ id: 38, filename: "typ-e-2024-02-26.log", date: "2024-02-26", view: "View", action: "Action" },
			{ id: 39, filename: "typ-e-2024-02-25.log", date: "2024-02-25", view: "View", action: "Action" },
			{ id: 40, filename: "typ-e-2024-02-24.log", date: "2024-02-24", view: "View", action: "Action" },
			{ id: 41, filename: "typ-e-2024-02-23.log", date: "2024-02-23", view: "View", action: "Action" },
			{ id: 42, filename: "typ-e-2024-02-22.log", date: "2024-02-22", view: "View", action: "Action" },
			{ id: 43, filename: "typ-e-2024-02-21.log", date: "2024-02-21", view: "View", action: "Action" },
			{ id: 44, filename: "typ-e-2024-02-20.log", date: "2024-02-20", view: "View", action: "Action" },
			{ id: 45, filename: "typ-e-2024-02-19.log", date: "2024-02-19", view: "View", action: "Action" },
			{ id: 46, filename: "typ-e-2024-02-18.log", date: "2024-02-18", view: "View", action: "Action" },
			{ id: 47, filename: "typ-e-2024-02-17.log", date: "2024-02-17", view: "View", action: "Action" },
			{ id: 48, filename: "typ-e-2024-02-16.log", date: "2024-02-16", view: "View", action: "Action" },
			{ id: 49, filename: "typ-e-2024-02-15.log", date: "2024-02-15", view: "View", action: "Action" },
			{ id: 50, filename: "typ-e-2024-02-14.log", date: "2024-02-14", view: "View", action: "Action" },
			{ id: 51, filename: "typ-e-2024-02-13.log", date: "2024-02-13", view: "View", action: "Action" },
			{ id: 52, filename: "typ-e-2024-02-12.log", date: "2024-02-12", view: "View", action: "Action" },
			{ id: 53, filename: "typ-e-2024-02-11.log", date: "2024-02-11", view: "View", action: "Action" },
			{ id: 54, filename: "typ-e-2024-02-10.log", date: "2024-02-10", view: "View", action: "Action" },
			{ id: 55, filename: "typ-e-2024-02-09.log", date: "2024-02-09", view: "View", action: "Action" },
			{ id: 56, filename: "typ-e-2024-02-08.log", date: "2024-02-08", view: "View", action: "Action" },
			{ id: 57, filename: "typ-e-2024-02-07.log", date: "2024-02-07", view: "View", action: "Action" },
			{ id: 58, filename: "typ-e-2024-02-06.log", date: "2024-02-06", view: "View", action: "Action" },
			{ id: 59, filename: "typ-e-2024-02-05.log", date: "2024-02-05", view: "View", action: "Action" },
			{ id: 60, filename: "typ-e-2024-02-04.log", date: "2024-02-04", view: "View", action: "Action" },
			{ id: 61, filename: "typ-e-2024-02-03.log", date: "2024-02-03", view: "View", action: "Action" },
			{ id: 62, filename: "typ-e-2024-02-02.log", date: "2024-02-02", view: "View", action: "Action" },
			{ id: 63, filename: "typ-e-2024-02-01.log", date: "2024-02-01", view: "View", action: "Action" },
			{ id: 64, filename: "typ-e-2024-01-31.log", date: "2024-01-31", view: "View", action: "Action" },
			{ id: 65, filename: "typ-e-2024-01-30.log", date: "2024-01-30", view: "View", action: "Action" },
			{ id: 66, filename: "typ-e-2024-01-29.log", date: "2024-01-29", view: "View", action: "Action" },
			{ id: 67, filename: "typ-e-2024-01-28.log", date: "2024-01-28", view: "View", action: "Action" },
			{ id: 68, filename: "typ-e-2024-01-27.log", date: "2024-01-27", view: "View", action: "Action" },
			{ id: 69, filename: "typ-e-2024-01-26.log", date: "2024-01-26", view: "View", action: "Action" },
			{ id: 70, filename: "typ-e-2024-01-25.log", date: "2024-01-25", view: "View", action: "Action" },
			{ id: 71, filename: "typ-e-2024-01-24.log", date: "2024-01-24", view: "View", action: "Action" },
			{ id: 72, filename: "typ-e-2024-01-23.log", date: "2024-01-23", view: "View", action: "Action" },
			{ id: 73, filename: "typ-e-2024-01-22.log", date: "2024-01-22", view: "View", action: "Action" },
			{ id: 74, filename: "typ-e-2024-01-21.log", date: "2024-01-21", view: "View", action: "Action" },
			{ id: 75, filename: "typ-e-2024-01-20.log", date: "2024-01-20", view: "View", action: "Action" },
			{ id: 76, filename: "typ-e-2024-01-19.log", date: "2024-01-19", view: "View", action: "Action" },
			{ id: 77, filename: "typ-e-2024-01-18.log", date: "2024-01-18", view: "View", action: "Action" },
			{ id: 78, filename: "typ-e-2024-01-17.log", date: "2024-01-17", view: "View", action: "Action" },
			{ id: 79, filename: "typ-e-2024-01-16.log", date: "2024-01-16", view: "View", action: "Action" },
			{ id: 80, filename: "typ-e-2024-01-15.log", date: "2024-01-15", view: "View", action: "Action" },
			{ id: 81, filename: "typ-e-2024-01-14.log", date: "2024-01-14", view: "View", action: "Action" },
			{ id: 82, filename: "typ-e-2024-01-13.log", date: "2024-01-13", view: "View", action: "Action" },
			{ id: 83, filename: "typ-e-2024-01-12.log", date: "2024-01-12", view: "View", action: "Action" },
			{ id: 84, filename: "typ-e-2024-01-11.log", date: "2024-01-11", view: "View", action: "Action" },
			{ id: 85, filename: "typ-e-2024-01-10.log", date: "2024-01-10", view: "View", action: "Action" },
			{ id: 86, filename: "typ-e-2024-01-09.log", date: "2024-01-09", view: "View", action: "Action" },
			{ id: 87, filename: "typ-e-2024-01-08.log", date: "2024-01-08", view: "View", action: "Action" },
			{ id: 88, filename: "typ-e-2024-01-07.log", date: "2024-01-07", view: "View", action: "Action" },
			{ id: 89, filename: "typ-e-2024-01-06.log", date: "2024-01-06", view: "View", action: "Action" },
			{ id: 90, filename: "typ-e-2024-01-05.log", date: "2024-01-05", view: "View", action: "Action" },
			{ id: 91, filename: "typ-e-2024-01-04.log", date: "2024-01-04", view: "View", action: "Action" },
			{ id: 92, filename: "typ-e-2024-01-03.log", date: "2024-01-03", view: "View", action: "Action" },
			{ id: 93, filename: "typ-e-2024-01-02.log", date: "2024-01-02", view: "View", action: "Action" },
			{ id: 94, filename: "typ-e-2024-01-01.log", date: "2024-01-01", view: "View", action: "Action" },
		]);

		// Dummy Values for Meta -> Replace with API fetch
		meta = {
			totalElements: 6,
			totalPages: Math.ceil(6 / params.size),
			to: 5,
			from: 0,
		};

		return {
			systemStore,
			columns,
			meta,
			params,
			logs: logs.value.slice(0, 6),
		};
	},
});

import { defineComponent, reactive, computed } from "vue";
import Table from "@/components/Table/table.vue";
import TableHeader from "@/components/Table/Header/tableHeader.vue";
import TableBody from "@/components/Table/Body/tableBody.vue";
import type { TableColumn } from "@/components/Table/_types/column";
import type { TableParams } from "@/components/Table/_types/params";
import type { TableMeta } from "@/components/Table/_types/meta";
import { useSystemStore } from "@/store/system/index.store";

// Components

export default defineComponent({
	name: "Settings",
	components: { Table, TableHeader, TableBody },
	setup() {
		const systemStore = useSystemStore();

		const columns: TableColumn[] = [
			{ key: "activity", label: "Activity", slot: "activity" },
			{ key: "state", label: "Status", slot: "state", sortable: false },
		];
		const rows = computed(() => [
			{
				id: 1,
				activity: {
					title: "Wlan",
					subtitle: "Activate or Deactivate Wlan on IOT-Access",
				},
				state: systemStore.wlan.status,
				networks: [
					{ ssid: "Test WLan 1", signal: -50 },
					{ ssid: "Test WLan 2", signal: -60 },
					{ ssid: "Test WLan 3", signal: -70 },
					{ ssid: "Test WLan 4", signal: -80 },
					{ ssid: "Test WLan 5", signal: -90 },
				], // Array<{ ssid: string; signal: number }>
			},
			{
				id: 2,
				activity: {
					title: "Logging",
					subtitle: "Erweitere Logs beinhalten Systemausgaben zur Fehleridentifizierung.",
				},
				state: systemStore.logging.state,
			},
		]);
		const tableParams = reactive<TableParams>({ page: 1, size: rows.value.length });

		// 4) Table-Meta
		const tableMeta = computed<TableMeta>(() => {
			const total = rows.value.length;
			return {
				totalElements: total,
				totalPages: Math.ceil(total / tableParams.size!),
				from: 1,
				to: total,
			};
		});

		// Network
		const networkColumns: TableColumn[] = [
			{ key: "ssid", label: "SSID", slot: "ssid", sortable: true },
			{ key: "signal", label: "Signal", slot: "signal", sortable: true },
		];
		const networkRows = computed(() => {
			return Array.from({ length: 55 }, (_, i) => ({
				id: i + 1,
				ssid: `Wlan ${i + 1}`,
				signal: `${-50 - i * 10} dBm`,
			}));
		});
		const networkParams = computed<TableParams>(() => ({
			page: 2,
			size: networkRows.value.length,
		}));
		const networkMeta = computed<TableMeta>(() => {
			const total = networkRows.value.length;
			const { page: p, size: s } = networkParams.value;
			const totalPages = Math.ceil(total / s);
			const from = (p - 1) * s + 1;
			const to = Math.min(p * s, total);
			return {
				totalElements: total,
				totalPages,
				from,
				to,
			};
		});

		const tableLoading = computed(() => systemStore.wlan.loading || systemStore.logging.loading);

		function onToggleCheckbox(row: any, key: "state", event: Event) {
			const input = event.target as HTMLInputElement;
			row[key] = input.checked;
			if (row.id === 1) {
				systemStore.wlan.status = input.checked;
			} else {
				systemStore.logging.state = input.checked;
			}
		}

		// 7) Paging-Event
		function onTableParamsChanged(newParams: TableParams) {
			Object.assign(tableParams, newParams);
		}

		return {
			systemStore,
			columns,
			rows,
			tableParams,
			tableMeta,

			networkColumns,
			networkRows,
			networkParams,
			networkMeta,

			tableLoading,
			onToggleCheckbox,
			onTableParamsChanged,
		};
	},
});

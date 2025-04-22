// src/components/Table/_types/column.ts
export interface TableColumn {
	key: string;
	label: string;
	sortable?: boolean;
	slot?: string;
	icon?: string;
	formatter?: (value: any, row: Record<string, any>) => string;
	clickable?: boolean;
}

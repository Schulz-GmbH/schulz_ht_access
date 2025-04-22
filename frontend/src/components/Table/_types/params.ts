export interface TableParams {
	size: number;
	page: number;
	recommendation?: string[];
}
export interface TableParamsWithSort extends TableParams {
	sortKey?: string;
	sortOrder?: "asc" | "desc" | "";
}

<template>
	<div class="flex-none w-full max-w-full px-3">
		<div
			class="relative flex flex-col min-w-0 mb-6 break-words bg-white border-0 border-transparent border-solid shadow-soft-xl rounded-2xl bg-clip-border">

			<TableHeader :title="title" :meta="meta" :params="params" @updateParams="onParamsChanged"
				:headerControl="headerControl" />
			<div class="flex-auto px-0 pt-0 pb-2">
				<div class="overflow-x-auto">
					<table class="items-center w-full mb-0 align-top border-gray-200 text-slate-500">
						<TableHead :columns="columns" @sort="toggleSort" :sortKey="sortKey" :sortOrder="sortOrder" />
						<TableBody :loading="loading" :data="pagedData" :columns="columns" :skeletonRows="size"
							@onColumnClicked="$emit('onColumnClicked', $event)">
							<template v-for="col in columns" #[col.slot]="{ row, column, value }">
								<slot :name="col.slot" :row="row" :column="column" :value="value" />
							</template>
						</TableBody>
					</table>
				</div>
			</div>

			<TableFooter :meta="meta" :params="params" @updateParams="onParamsChanged" :footerControl="footerControl" />

		</div>
	</div>
</template>


<script lang="ts">

export { default } from './table'

</script>

<style src="./table.scss"></style>

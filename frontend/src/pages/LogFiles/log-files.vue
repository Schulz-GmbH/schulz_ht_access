<template>
	<div class="flex flex-wrap my-6 -mx-3">
		<Table title="Last 6 Log Files" :data="logs" :columns="columns" :meta="meta" :params="params"
			:headerControl="true" :footerControl="false" @update:params="onParamsChange">
			<!-- Named slot "view" -->
			<template #view="{ row }">
				<button type="button" className="text-slate-700 mr-2 flex h-8 w-8 items-center justify-center"
					:title="`View ${row.filename}`" @click="viewLog(row.filename)">
					<i class="fas fa-eye"></i>
				</button>

			</template>

			<!-- Named slot "action" -->
			<template #action="{ row }">
				<button type="button" :title="`Delete ${row.filename}`"
					className="bg-gradient-to-tl from-red-700 to-red-500 text-white shadow-soft-2xl mr-2 flex h-8 w-8 items-center justify-center"
					@click="deleteLog(row.filename)">
					<i class="fas fa-trash-alt"></i>
				</button>
			</template>
		</Table>
	</div>
</template>

<script lang="ts">
export { default } from './log-files'

</script>

<style src="./log-files.scss"></style>

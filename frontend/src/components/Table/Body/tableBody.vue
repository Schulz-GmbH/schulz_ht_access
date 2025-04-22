<template>
	<tbody>
		<template v-if="loading">
			<tr v-for="n in skeletonRows" :key="n">
				<td class="p-4 align-middle bg-transparent border-b border-gray-200 whitespace-nowrap shadow-transparent"
					v-for="col in columns" :key="col.key">
					<div class="h-4 bg-gray-300 rounded animate-pulse w-3/4"></div>
				</td>
			</tr>
		</template>
		<template v-else>
			<tr v-for="(row, rowIndex) in data" :key="row.id || rowIndex">
				<td v-for="(col, colIndex) in columns" :key="col.key || colIndex" @click="handleColumnClicked(row, col)"
					class="p-4 align-middle bg-transparent border-b border-gray-200 whitespace-nowrap shadow-transparent">
					<div class="flex px-2">
						<div class="my-auto">
							<h6 v-if="col.slot" :is="col.slot" :row="row" :column="col"
								class="mb-0 text-sm leading-normal"></h6>
							<h6 v-else class="mb-0 text-sm leading-normal">{{ row[col.key] }}</h6>
						</div>
					</div>
				</td>
			</tr>
		</template>
	</tbody>
</template>
<script lang="ts">

export { default } from './tableBody'

</script>

<style src="./tableBody.scss"></style>

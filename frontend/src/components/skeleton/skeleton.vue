<template>
	<div class="flex flex-row -mx-3 animate-pulse" v-if="isLoading">
		<div class="flex-none w-2/3 max-w-full px-3">
			<div class="space-y-2">
				<div v-for="(r, index) in rows" :key="index" class="h-4 bg-gray-300 rounded"
					:style="{ width: randomWidths[index] }"></div>
			</div>
		</div>
		<div class="flex justify-end px-3 basis-1/3" v-if="withIcon">
			<div class="flex items-center justify-center w-12 h-12 rounded-lg bg-gray-300"></div>
		</div>
	</div>
</template>

<script lang="ts">

export { default } from './skeleton'

</script>

<style src="./skeleton.scss"></style>

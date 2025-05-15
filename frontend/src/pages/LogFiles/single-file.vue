<template>
	<div class="h-full p-4 space-y-4">
		<!-- Toolbar -->
		<div class="flex items-center justify-between mb-4">
			<div class="space-x-2">
				<button @click="rename"
					class="px-3 py-1 rounded shadow-sm text-sm border border-gray-400 text-gray-700 hover:bg-gray-100">
					<i class="fas fa-edit"></i> Umbenennen
				</button>
				<button @click="remove"
					class="px-3 py-1 rounded shadow-sm text-sm bg-red-500 text-white hover:bg-red-600">
					<i class="fas fa-trash"></i> Löschen
				</button>
				<button @click="send"
					class="px-3 py-1 rounded shadow-sm text-sm bg-blue-600 text-white hover:bg-blue-700">
					<i class="fas fa-paper-plane"></i> Versenden
				</button>
			</div>
		</div>

		<!-- Log-Content -->
		<pre class="h-full bg-gray-50 py-4 rounded overflow-auto whitespace-pre-wrap">{{ content }}</pre>
	</div>
</template>


<script lang="ts">
export { default } from './single-file'

</script>

<style src="./single-file.scss" scoped></style>

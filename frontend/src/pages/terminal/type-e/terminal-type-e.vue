<template>
	<terminal />
	<mobile-keyboard />
	<Modal v-model="showModal" @confirm="onFilenameConfirm">
		<template #header>Log speichern unter</template>
		<input v-model="filename" placeholder="Dateiname" class="w-full border px-2 py-1 rounded" />
		<template #footer>
			<button class="btn" @click="showModal = false">Abbrechen</button>
			<button class="btn btn-primary" @click="onFilenameConfirm">
				Speichern
			</button>
		</template>
	</Modal>
</template>

<script lang="ts">
export { default } from './terminal-type-e'
</script>

<style src="./terminal-type-e.scss"></style>

<style lang="scss" scoped></style>

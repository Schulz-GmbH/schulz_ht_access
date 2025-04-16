<template>
	<div>
		<div v-if="!onlineStatus" class="offline-banner">
			Offline
		</div>
	</div>

</template>

<script lang="ts">
export { default } from './offline.component'
</script>


<style src="./offline.component.scss"></style>

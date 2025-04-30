<template>
	<div id="network-status">
		<NetworkStatusBanner />
	</div>
	<div id="basis-layout" class="relative h-full flex flex-col flex-1 overflow-hidden">
		<AppLayout />
	</div>
</template>

<script lang="ts">

export { default } from './basis.layout'

</script>

<style src="./basis.layout.scss"></style>

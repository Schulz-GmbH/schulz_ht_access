<template>
	<section class="app-wrapper" :class="layoutClasses">
		<div v-if="layoutClasses.mobile && layoutClasses.openNavbar" class="drawer-bg" @click="handleClickOutside" />
		<router-view />
	</section>
</template>

<script lang="ts">

export { default } from './app.layout'

</script>

<style src="./app.layout.scss"></style>

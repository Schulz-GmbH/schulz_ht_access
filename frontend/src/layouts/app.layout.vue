<template>
	<section class="app-wrapper flex flex-1 flex-col relative h-full" :class="layoutClasses">
		<div v-if="mobile && openNavbar" class="drawer-bg absolute inset-0 w-full h-full bg-transparent z-[999]"
			@click="handleClickOutside" />
		<Sidebar class="flex-none" />

		<!-- <main class="flex flex-col xl:ml-68.5 relative rounded-xl ease-soft-in-out transition-all duration-200"> -->
		<NavigationBar class="flex-none" />
		<main class="main-content flex flex-col h-full overflow-auto">
			<div class="flex flex-col flex-1 min-h-0 w-full px-6 py-6 mx-auto">
				<router-view />
			</div>
		</main>
		<Footer class="flex-none" />
	</section>
</template>

<script lang="ts">

export { default } from './app.layout'

</script>

<style src="./app.layout.scss"></style>
